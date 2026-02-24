// Fill out your copyright notice in the Description page of Project Settings.


#include "SlingShot.h"
// 맨 위에 지오메트리 스크립트로 원기둥을 만들기 위한 헤더 추가!
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "Math/Plane.h" // 3D 공간 마우스 투영을 위한 수학 계산용
#include "Engine/World.h"
#include "Base_Bird.h"

// Sets default values
ASlingShot::ASlingShot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. Root Component 생성 및 지정
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	// 2. 새총 몸통 생성 및 Root에 부착
	SlingshotBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlingshotBody"));
	SlingshotBody->SetupAttachment(RootComp);

	// 3. 파우치(가죽) 생성 및 Root에 부착
	Pouch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pouch"));
	Pouch->SetupAttachment(RootComp);

	// 4. 왼쪽 고무줄(Dynamic Mesh) 생성 및 Root에 부착
	LeftBand = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("LeftBand"));
	LeftBand->SetupAttachment(RootComp);

	// 5. 오른쪽 고무줄(Dynamic Mesh) 생성 및 Root에 부착
	RightBand = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("RightBand"));
	RightBand->SetupAttachment(RootComp);
    
	// --- 변수 초기값 설정 ---
	bIsAiming = false;
	bIsReturning = false; //이걸 해줘야 게임 시작시 파우치 흔들리는거 방지
	PullPower = 0.1f; // 당기는 힘 초기 값 = 최대값으로 시작 (조절 가능)
    
	// 탄성 애니메이션 관련 변수 초기화 초기 속도 0으로 시작
	PouchVelocity = FVector::ZeroVector;

}

// Called when the game starts or when spawned
void ASlingShot::BeginPlay()
{
	Super::BeginPlay();
	LoadBird();
	if (Pouch) // 파우치가 제대로 생성되었는지 확인한 후, 초기 위치를 저장합니다. 이 위치가 탄성 애니메이션의 기준점이 됩니다.
	{
		DefaultPouchLocation = Pouch->GetRelativeLocation(); // 파우치의 초기 위치를 저장 (RootComp 기준)
	}
}

// Called every frame
void ASlingShot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAiming) // 조준 중일 때, 마우스 위치에 따라 파우치 위치 업데이트, 고무줄 업데이트
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController(); // 플레이어 컨트롤러 가져오기 (조준 계산에 필요)
		UpdateAim(PC); // 마우스 위치에 따라 파우치 위치 업데이트
		UpdateBands(); // 고무줄 업데이트
        
		// 조준 중일 때는 파우치를 꽉 잡고 있으므로 속도를 0
		PouchVelocity = FVector::ZeroVector; 
	}
	else 
	{
		// 조준이 끝나고 파우치가 원래 위치로 돌아가는 탄성 애니메이션 계산
        
		// 1. 파우치가 쉬는 위치
		FVector RestPosition = DefaultPouchLocation + FVector(0, 0, -30.0f);
		FVector CurrentPosition = Pouch->GetRelativeLocation();

		// 2. 스프링 계수 (Tension: 당기는 힘, Damping: 브레이크 거는 힘)
		float Tension = 800.0f; // 숫자가 클수록 빠르고 강하게 튕깁니다. 100~2000
		float Damping = 15.0f;  // 숫자가 클수록 튕김이 빨리 멈춥니다. 2~50

		// 3. 훅의 법칙 (Hooke's Law) 기반 가속도 계산
		FVector SpringForce = (RestPosition - CurrentPosition) * Tension; // (현재 위치 - 쉬는 위치) * 장력 = 스프링 힘
		FVector DampingForce = -PouchVelocity * Damping; // 파우치의 속도 * 감쇠력 = 감쇠력 (속도가 빠를수록 더 큰 브레이크가 걸립니다)
		FVector Acceleration = SpringForce + DampingForce; // 총 가속도 = 스프링 힘 + 감쇠력

		// 4. 속도와 위치 업데이트
		PouchVelocity += Acceleration * DeltaTime;
		FVector NewLoc = CurrentPosition + (PouchVelocity * DeltaTime);

		Pouch->SetRelativeLocation(NewLoc);
		UpdateBands();
	}
}
// 3. OnConstruction 함수의 맨 아래쪽에 머티리얼 적용 코드 한 줄 추가
// 고무줄의 머테리얼을 BandMaterial로 설정 후 업데이트 고무줄의 늘어난 정도에 따라 색상 변화 등을 BandMaterial에서 머티리얼 파라미터로 조절할 수 있게 됩니다.
void ASlingShot::OnConstruction(const FTransform& Transform) 

{
    Super::OnConstruction(Transform);
    UpdateBands();
    
    // 
    if (BandMaterial) // BandMaterial 설정 되어 있으면 양쪽 고무줄에 머티리얼 적용
    {
       LeftBand->SetMaterial(0, BandMaterial);
       RightBand->SetMaterial(0, BandMaterial);
    }
}

void ASlingShot::UpdateAim(APlayerController* PlayerController)
{
    
    //조준중이거나 플레이어 컨트롤 및 카메라, 파우치, 새총 몸통이 모두 유효한지 확인하는 방어 코드입니다.
    if (!bIsAiming || !PlayerController || !PlayerController->PlayerCameraManager || !Pouch || !SlingshotBody) return;

    FVector MouseWorldLoc, MouseWorldDir; //마우스의 월드 위치와 방향 벡터 저장

    // 만약 마우스 위치를 월드 좌표로 변환하면 DeprojectMousePositionToWorld를 통해서 스크린 좌표인 마우스 위치를 월드 좌표로 변환할 수 있습니다. 
    // 이 함수는 마우스의 위치를 기준으로 카메라에서 시작하는 레이저(광선)의 시작점과 방향을 반환합니다.
    //DeprojectMousePosition 은 마우스의 스크린 좌표를 월드 좌표로 반환하는 함수
    if (PlayerController->DeprojectMousePositionToWorld(MouseWorldLoc, MouseWorldDir)) // 마우스 위치를 월드 좌표로 변환하는 데 성공했다면
    {
        // 새총 몸통의 위치를 가상 평면의 기준점으로 잡는다.
       FVector SlingshotLoc = SlingshotBody->GetComponentLocation();
       
       // 평면의 법선 벡터를 '카메라가 바라보는 방향'으로 설정
       FVector PlaneNormal = PlayerController->PlayerCameraManager->GetCameraRotation().Vector();
       PlaneNormal.Normalize(); 
        
       // 마우스 레이저가 가상 평면과 만나는 3D 좌표 구하기
       FVector IntersectionPoint = FMath::LinePlaneIntersection(
          MouseWorldLoc, 
          MouseWorldLoc + (MouseWorldDir * 10000.0f), 
          SlingshotLoc, 
          PlaneNormal
       );

       // 파우치의 기준점(원점)을 월드 좌표로 가져오기
       FVector OriginLocation = RootComp->GetComponentTransform().TransformPosition(DefaultPouchLocation);
       
       // 마우스가 있는 지점으로 향하는 당김 벡터 계산
       FVector DragVector = IntersectionPoint - OriginLocation;

       // 당김 벡터의 길이가 PullPower보다 크면, 벡터를 정규화한 후 PullPower 길이로 제한
       if (DragVector.Size() > PullPower) 
       {
          DragVector = DragVector.GetSafeNormal() * PullPower;
       }

       // 6. 기준점에서 당긴 만큼 이동시킨 최종 위치 적용
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

            

            // ★ 방향 벡터 계산 후, 길이가 0인지 검사하는 방어 코드 추가!
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

            // 6. 지오메트리 트랜스폼 세팅 
            FTransform LeftCylTransform(LeftRotator, LeftHornLocal);
            FTransform RightCylTransform(RightRotator, RightHornLocal);

            // 길이는 당긴 만큼, 반지름은 길이에 반비례하도록 설정
            float LeftRadius = FMath::Clamp(20.0f * (20.0f / FMath::Max(LeftLength, 1.0f)), 0.3f, 10.0f);
            float RightRadius = FMath::Clamp(20.0f * (20.0f / FMath::Max(RightLength, 1.0f)), 0.3f, 10.0f);
            // 두께나 길이가 너무 작으면 아예 그리지 않도록 방어
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
        // 화면에 빨간색 에러 메시지 띄우기 (문제가 뭔지 바로 알 수 있습니다)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("에러: BirdClass가 비어있거나 파우치가 없습니다!"));
        return;
    }

    FVector SpawnLocation = Pouch->GetComponentLocation();
    FRotator SpawnRotation = Pouch->GetComponentRotation();

    // ★ 추가: 충돌을 무시하고 무조건 강제 스폰하는 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 변수들을 넣어서 스폰  
    CurrentBird = GetWorld()->SpawnActor<AActor>(BirdClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (CurrentBird)
    {
        // 화면에 초록색 성공 메시지 띄우기
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("스폰 성공! 파우치에 부착합니다."));

        // ★ 수정된 코드: 맨 끝에 FName("LaunchPouch")를 추가합니다!
        CurrentBird->AttachToComponent(Pouch, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LaunchPouch"));
    }
}

void ASlingShot::FireBird()
{
    // 장전된 새가 없으면 쏠 수 없음
    if (!CurrentBird) return;

    UE_LOG(LogTemp,Warning, TEXT("FireBird 함수 실행!")); // 디버깅용 로그
    // 1. 파우치에서 새를 분리합니다. (KeepWorldTransform: 분리되는 순간의 월드 위치를 그대로 유지)
    CurrentBird->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    // 2. 🚀 발사 방향과 힘(가속도) 계산하기
    // 새총의 중심(쉬는 위치) 월드 좌표
    FVector OriginLocation = RootComp->GetComponentTransform().TransformPosition(DefaultPouchLocation);
    // 현재 파우치 좌표
    FVector CurrentPouchLoc = Pouch->GetSocketLocation(FName("LaunchPouch")); // (이걸로 수정 추천)
    
    // 발사 벡터 = (목표 위치 - 현재 위치) -> 파우치에서 중심을 향하는 방향!
    FVector PullVector = OriginLocation - CurrentPouchLoc;

    // 당긴 거리에 비례해서 던질 힘을 증폭시킵니다. (Multiplier는 테스트하며 조절)
    float ForceMultiplier = 500.0f; 
    FVector LaunchVelocity = PullVector * ForceMultiplier;

    // 3. 팀원에게 계산된 힘 넘겨주기!
    
     
      ABase_Bird* MyBird = Cast<ABase_Bird>(CurrentBird);
      if (MyBird) {
		MyBird->Launch(LaunchVelocity); // 팀원이 물리학적 계산을 처리할 함수
      	UE_LOG(LogTemp, Warning, TEXT("발사 벡터: %s"), *LaunchVelocity.ToString());
      }
	//
     

    // ----- [임시 테스트용 코드] -----
    // 팀원 코드가 아직 없다면, 언리얼 기본 물리 시스템으로 밀어버려서 잘 날아가는지 테스트해 볼 수 있습니다.
    // UStaticMeshComponent* BirdMesh = Cast<UStaticMeshComponent>(CurrentBird->GetComponentByClass(UStaticMeshComponent::StaticClass()));
    // if (BirdMesh)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("발사 벡터: %s"), *LaunchVelocity.ToString()); // 발사 벡터 로그로 출력 (디버깅용)
    //     BirdMesh->SetSimulatePhysics(true); // 물리를 켜고
    //     BirdMesh->AddImpulse(LaunchVelocity, NAME_None, true); // 힘을 가합니다 (true = 질량 무시하고 속도 즉시 변경)
    // }
    // --------------------------------

    // 발사했으니 현재 장전된 새는 없다고 비워줍니다.
    CurrentBird = nullptr;
}
void ASlingShot::PullString()
{
    bIsAiming = true; // 조준 시작 (Tick에서 파우치가 마우스를 따라가기 시작함)
}

void ASlingShot::ReleaseString()
{
    // 1. 조준 상태를 꺼서 파우치가 원래 자리로 튕겨 돌아가게 만듦
    bIsAiming = false; 

    // 2. ★ 핵심: 새를 분리하고 물리력을 가해 날려보내는 함수 실행!
    FireBird(); 
}

void ASlingShot::IncreasePower()
{
    // 파워를 50씩 증가시키고, 최대 1000까지만 커지게 제한합니다.
    PullPower = FMath::Clamp(PullPower + 50.0f, 1.0f, 1000.0f);
    
    // 에디터 화면에 현재 파워가 얼만지 파란색 글씨로 띄워줍니다!
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("파워 증가! 현재 최대 파워: %f"), PullPower));
}

void ASlingShot::DecreasePower()
{
    // 파워를 50씩 감소시키고, 최소 100 이하로는 안 내려가게 제한합니다.
    PullPower = FMath::Clamp(PullPower - 50.0f, 100.0f, 1000.0f);
    
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("파워 감소! 현재 최대 파워: %f"), PullPower));
}
