#include "SlingShot.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "Math/Plane.h" 
#include "Engine/World.h"
#include "Base_Bird.h"

ASlingShot::ASlingShot()
{
    PrimaryActorTick.bCanEverTick = true;
    
    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
    RootComponent = RootComp;

    SlingshotBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlingshotBody"));
    SlingshotBody->SetupAttachment(RootComp);

    Pouch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pouch"));
    Pouch->SetupAttachment(RootComp);

    LeftBand = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("LeftBand"));
    LeftBand->SetupAttachment(RootComp);

    RightBand = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("RightBand"));
    RightBand->SetupAttachment(RootComp);
    
    bIsAiming = false;
    bIsReturning = false; 
    PullPower = 0.1f; 
    
    PouchVelocity = FVector::ZeroVector;
}

void ASlingShot::BeginPlay()
{
    Super::BeginPlay();
    LoadBird();
    if (Pouch) 
    {
       DefaultPouchLocation = Pouch->GetRelativeLocation(); 
    }
}

void ASlingShot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsAiming) 
    {
       APlayerController* PC = GetWorld()->GetFirstPlayerController(); 
       UpdateAim(PC); 
       UpdateBands(); 
       PouchVelocity = FVector::ZeroVector; 
    }
    else 
    {
       FVector RestPosition = DefaultPouchLocation;
       FVector CurrentPosition = Pouch->GetRelativeLocation();

       float Tension = 80.0f; 
       float Damping = 5.0f;  

       FVector SpringForce = (RestPosition - CurrentPosition) * Tension; 
       FVector DampingForce = -PouchVelocity * Damping; 
       FVector Acceleration = SpringForce + DampingForce; 

       PouchVelocity += Acceleration * DeltaTime;
       FVector NewLoc = CurrentPosition + (PouchVelocity * DeltaTime);

       Pouch->SetRelativeLocation(NewLoc);
       UpdateBands();
       
       if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Q))
       {
          TriggerBirdAbility();
          CurrentBird = nullptr; 
       }
    }
}

void ASlingShot::OnConstruction(const FTransform& Transform) 
{
    Super::OnConstruction(Transform);
    UpdateBands();
    
    if (BandMaterial) 
    {
       LeftBand->SetMaterial(0, BandMaterial);
       RightBand->SetMaterial(0, BandMaterial);
    }
}

// 매 프레임 마우스 커서 위치와 휠 값을 받아와 파우치 위치를 갱신하는 함수
void ASlingShot::UpdateAim(APlayerController* PlayerController)
{
    if (!bIsAiming || !PlayerController || !Pouch || !SlingshotBody) return;

    FVector MouseWorldLoc, MouseWorldDir; // MouseWorldLoc 은 마우스의 월드 위치 MouseWorldDir 는 마우스에서 쏘아진 광선의 방향 벡터입니다.
    // DeprojectMousePositionToWorld() 함수는 화면상의 마우스 위치를 월드 공간의 위치와 방향으로 변환해줍니다..
    if (PlayerController->DeprojectMousePositionToWorld(MouseWorldLoc, MouseWorldDir))
    {
        FVector SlingshotLoc = SlingshotBody->GetComponentLocation(); // 새총의 월드 위치
        // FVector PlaneNormal = PlayerController->PlayerCameraManager->GetCameraRotation().Vector(); // 카메라 방향의 법선 벡터.
        FVector PlaneNormal = -RootComp->GetForwardVector(); // 새총의 앞 방향을 평면의 법선으로 사용 (카메라 방향 대신)
        FVector OffsetPlanePoint = SlingshotLoc - (RootComp->GetForwardVector() * 150.0f);

        FVector CurrentIntersection = FMath::LinePlaneIntersection(
            MouseWorldLoc, 
            MouseWorldLoc + (MouseWorldDir * 10000.0f), 
            OffsetPlanePoint, 
            PlaneNormal
        );
        DrawDebugLine( 
            GetWorld(), 
            MouseWorldLoc, 
            MouseWorldLoc + (MouseWorldDir * 10000.0f), 
            FColor::Green, 
            false, 
            -1.0f,
            0, 
            2.0f
        );

        // StartAimLocation 은 PullString() 함수에서 마우스 클릭 시점의 교차점을 기록한 위치입니다.
        FVector WorldDelta = CurrentIntersection - StartAimLocation;

        // 3. 이 차이값을 슬링샷의 로컬 좌표계로 변환 (상하좌우 방향 추출)
        FVector LocalDelta = RootComp->GetComponentTransform().InverseTransformVector(WorldDelta);

        // 4. 최종 로컬 위치 조립
        // Local X: 휠로 조절하는 PullPower (뒤로 당겨지는 힘)
        // Local Y, Z: 마우스 드래그로 조절하는 상하좌우 조준
        FVector NewLocalPouchLoc;
        NewLocalPouchLoc.X = DefaultPouchLocation.X - PullPower; // 휠로 조절된 값만큼 뒤로
        NewLocalPouchLoc.Y = DefaultPouchLocation.Y + LocalDelta.Y; // 마우스 좌우 오프셋
        NewLocalPouchLoc.Z = DefaultPouchLocation.Z + LocalDelta.Z; // 마우스 상하 오프셋
        
        if (GEngine)
        {
            FString DebugMsg = FString::Printf(TEXT("Mouse Z Delta: %f / Final Z: %f"), LocalDelta.Z, NewLocalPouchLoc.Z);
            FString DebugMsgY = FString::Printf(TEXT("Mouse Y Delta: %f / Final Y: %f"), LocalDelta.Y, NewLocalPouchLoc.Y);
            GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, DebugMsg);
            GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Yellow, DebugMsgY);
        }

        // 5. 상하좌우 조준 제한 (너무 많이 꺾이지 않게) // Clamp( 값, 최소, 최대 ) 함수를 사용하여 Y와 Z축의 위치를 제한합니다. 이 범위는 필요에 따라 조절할 수 있습니다.
        float ClampRange = 150.0f; // 이동 가능 범위

        NewLocalPouchLoc.Y = FMath::Clamp(NewLocalPouchLoc.Y, 
            DefaultPouchLocation.Y - ClampRange, 
            DefaultPouchLocation.Y + ClampRange);

        NewLocalPouchLoc.Z = FMath::Clamp(NewLocalPouchLoc.Z, 
            DefaultPouchLocation.Z - ClampRange, 
            DefaultPouchLocation.Z + ClampRange);

        // 6. 월드 위치로 변환하여 적용
        FVector NewWorldLoc = RootComp->GetComponentTransform().TransformPosition(NewLocalPouchLoc);
        Pouch->SetWorldLocation(NewWorldLoc);
        // Pouch->SetWorldLocation(NewWorldLoc, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

void ASlingShot::UpdateBands()
{
    if (LeftBand && RightBand && SlingshotBody && Pouch)
    {
        UDynamicMesh* LeftMesh = LeftBand->GetDynamicMesh();
        UDynamicMesh* RightMesh = RightBand->GetDynamicMesh();

        if (LeftMesh && RightMesh)
        {
            LeftMesh->Reset();
            RightMesh->Reset();

            FGeometryScriptPrimitiveOptions Options;
            
            FVector LeftHornWorld = SlingshotBody->GetSocketLocation(FName("LeftHorn"));
            FVector RightHornWorld = SlingshotBody->GetSocketLocation(FName("RightHorn"));
            FVector LeftPouchWorld = Pouch->GetSocketLocation(FName("LeftPouch"));
            FVector RightPouchWorld = Pouch->GetSocketLocation(FName("RightPouch"));

            FTransform LeftBandWorldTrans = LeftBand->GetComponentTransform();
            FVector LeftHornLocal = LeftBandWorldTrans.InverseTransformPosition(LeftHornWorld);
            FVector LeftPouchLocal = LeftBandWorldTrans.InverseTransformPosition(LeftPouchWorld);

            FTransform RightBandWorldTrans = RightBand->GetComponentTransform();
            FVector RightHornLocal = RightBandWorldTrans.InverseTransformPosition(RightHornWorld);
            FVector RightPouchLocal = RightBandWorldTrans.InverseTransformPosition(RightPouchWorld);

            float LeftLength = FVector::Dist(LeftHornLocal, LeftPouchLocal);
            float RightLength = FVector::Dist(RightHornLocal, RightPouchLocal);

            FVector LeftDir = LeftPouchLocal - LeftHornLocal;
            FVector RightDir = RightPouchLocal - RightHornLocal;

            FRotator LeftRotator = FRotator::ZeroRotator;
            if (!LeftDir.IsNearlyZero())
            {
                LeftRotator = FRotationMatrix::MakeFromZ(LeftDir).Rotator();
            }

            FRotator RightRotator = FRotator::ZeroRotator;
            if (!RightDir.IsNearlyZero())
            {
                RightRotator = FRotationMatrix::MakeFromZ(RightDir).Rotator();
            }

            FTransform LeftCylTransform(LeftRotator, LeftHornLocal);
            FTransform RightCylTransform(RightRotator, RightHornLocal);

            float LeftRadius = FMath::Clamp(20.0f * (20.0f / FMath::Max(LeftLength, 1.0f)), 0.3f, 10.0f);
            float RightRadius = FMath::Clamp(20.0f * (20.0f / FMath::Max(RightLength, 1.0f)), 0.3f, 10.0f);
            
            if (LeftLength > 1.0f)
            {
                UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCylinder(
                    LeftMesh, Options, LeftCylTransform, LeftRadius, LeftLength
                );
            }

            if (RightLength > 1.0f)
            {
                UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendCylinder(
                    RightMesh, Options, RightCylTransform, RightRadius, RightLength
                );
            }
        }
    }
}

void ASlingShot::LoadBird()
{
    if (!BirdClass || !Pouch) 
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("에러: BirdClass가 비어있거나 파우치가 없습니다!"));
        return;
    }

    FVector SpawnLocation = Pouch->GetComponentLocation();
    FRotator SpawnRotation = Pouch->GetComponentRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CurrentBird = GetWorld()->SpawnActor<AActor>(BirdClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (CurrentBird)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("스폰 성공! 파우치에 부착합니다."));
        CurrentBird->AttachToComponent(Pouch, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LaunchPouch"));
        CurrentBird->SetActorRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    }
}

void ASlingShot::FireBird()
{
    if (!CurrentBird) return;

    UE_LOG(LogTemp,Warning, TEXT("FireBird 함수 실행!")); 
    CurrentBird->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    FVector OriginLocation = RootComp->GetComponentTransform().TransformPosition(DefaultPouchLocation);
    FVector CurrentPouchLoc = Pouch->GetSocketLocation(FName("LaunchPouch")); 
    
    FVector PullVector = OriginLocation - CurrentPouchLoc;

    float ForceMultiplier = 50.0f; 
    FVector LaunchVelocity = PullVector * ForceMultiplier;

    // [수정된 3번 로직] 팀원에게 힘 넘겨주기 + 카메라 전환
    ABase_Bird* MyBird = Cast<ABase_Bird>(CurrentBird);
    if (MyBird) {
       // 카메라 복귀 시 바라볼 타겟을 이 새총(this)으로 지정
       MyBird->ReturnTarget = this;

       // 발사 시점에 플레이어 카메라를 새의 시점으로 0.5초간 부드럽게 전환
       APlayerController* PC = GetWorld()->GetFirstPlayerController();
       if (PC)
       {
           PC->SetViewTargetWithBlend(MyBird, 0.5f);
       }

       MyBird->Launch(LaunchVelocity); 
       UE_LOG(LogTemp, Warning, TEXT("발사 벡터: %s"), *LaunchVelocity.ToString());
    }
}

// 마우스 클릭을 하면 실행 되는 함수
void ASlingShot::PullString()
{
    bIsAiming = true;
    if (Pouch)
    {
        Pouch->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    
    // 클릭 시점 마우스의 위치를 월드 좌표로 변환하여 StartAimLocation에 저장하는 로직
    APlayerController* PC = GetWorld()->GetFirstPlayerController(); // 플레이 컨트롤러 가져오기 (현재 월드의 첫 번째 플레이어 컨트롤러)
    if (PC && PC->PlayerCameraManager) // 플레이어 컨트롤과 카메라 매니저가 유효한지 확인 카메라 매니지는 카메라의 방향 벡터를 얻기 위해 필요합니다.
    {
        FVector MouseWorldLoc, MouseWorldDir; // MouseWorldLoc 은 마우스의 월드 위치 MouseWorldDir
        if (PC->DeprojectMousePositionToWorld(MouseWorldLoc, MouseWorldDir))
        {
            FVector SlingshotLoc = SlingshotBody->GetComponentLocation(); // 새총의 월드 위치
            // FVector PlaneNormal = PC->PlayerCameraManager->GetCameraRotation().Vector(); // 카메라 방향의 법선 벡터
            FVector PlaneNormal = -RootComp->GetForwardVector();
            // 새총과 파우치 사이의 평면 (OffsetPlanePoint는 새총에서 뒤로 150만큼 떨어진 지점)
            FVector OffsetPlanePoint = SlingshotLoc - (RootComp->GetForwardVector() * 150.0f);
            // 클릭 시점의 교차점을 기준점으로 기록
            // LinePlaneIntersection 함수를 사용하여 마우스에서 쏘아진 광선과 평면의 교차점을 계산 LinePlaneIntersection(광선 시작점, 광선 방향, 평면의 한 점, 평면의 법선 벡터) 
            // 여기서 MouseWorldDir * 10000.0f는 광선을 충분히 멀리 쏘아주는 역할을 합니다. 이 광선과 평면의 교차점이 StartAimLocation이 됩니다.
            StartAimLocation = FMath::LinePlaneIntersection(
                MouseWorldLoc, 
                MouseWorldLoc + (MouseWorldDir * 10000.0f), 
                OffsetPlanePoint, 
                PlaneNormal
            );
        }
    }
}

void ASlingShot::ReleaseString()
{
    bIsAiming = false; 
    FireBird(); 
}

void ASlingShot::IncreasePower()
{
    PullPower = FMath::Clamp(PullPower + 50.0f, 1.0f, 1000.0f);
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("파워 증가! 현재 최대 파워: %f"), PullPower));
}

void ASlingShot::DecreasePower()
{
    PullPower = FMath::Clamp(PullPower - 50.0f, 100.0f, 1000.0f);
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("파워 감소! 현재 최대 파워: %f"), PullPower));
}

void ASlingShot::TriggerBirdAbility()
{
    if (CurrentBird)
    {
       ABase_Bird* MyBird = Cast<ABase_Bird>(CurrentBird);
       if (MyBird && !MyBird->bAbilityUsed)
       {
          MyBird->UseAbility();
          MyBird->bAbilityUsed = true; 
          GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("새 능력 발동!"));
       }
    }
}