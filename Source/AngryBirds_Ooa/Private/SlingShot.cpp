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
       FVector RestPosition = DefaultPouchLocation + FVector(0, 0, -30.0f);
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

void ASlingShot::UpdateAim(APlayerController* PlayerController)
{
    if (!bIsAiming || !PlayerController || !PlayerController->PlayerCameraManager || !Pouch || !SlingshotBody) return;

    FVector MouseWorldLoc, MouseWorldDir; 
    if (PlayerController->DeprojectMousePositionToWorld(MouseWorldLoc, MouseWorldDir)) 
    {
       FVector SlingshotLoc = SlingshotBody->GetComponentLocation();
       FVector PlaneNormal = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
       PlaneNormal.Normalize(); 
        
       FVector IntersectionPoint = FMath::LinePlaneIntersection(
          MouseWorldLoc, 
          MouseWorldLoc + (MouseWorldDir * 10000.0f), 
          SlingshotLoc, 
          PlaneNormal
       );

       FVector OriginLocation = RootComp->GetComponentTransform().TransformPosition(DefaultPouchLocation);
       FVector DragVector = IntersectionPoint - OriginLocation;

       if (DragVector.Size() > PullPower) 
       {
          DragVector = DragVector.GetSafeNormal() * PullPower;
       }

       FVector NewPouchLoc = OriginLocation + DragVector;
       Pouch->SetWorldLocation(NewPouchLoc);
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

void ASlingShot::PullString()
{
    bIsAiming = true; 
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