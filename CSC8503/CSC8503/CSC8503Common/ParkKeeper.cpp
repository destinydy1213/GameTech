#include "ParkKeeper.h"
#include "GameWorld.h"

using namespace NCL::CSC8503;

ParkKeeper::ParkKeeper(string objectName) {
	name = objectName;
	isActive = true;
	boundingVolume = nullptr;
	physicsObject = nullptr;
	renderObject = nullptr;
	networkObject = nullptr;
	world = nullptr;
	timer = new GameTimer();
	deltaTime = 0.0f;
	firstTime = true;
}

void ParkKeeper::setStateMachine() {
	keeperStateMachine = new StateMachine();
	SetPlayer((PlayerGoose*)this->GetGameWorld()->GetPlayer());
	distance = (this->GetGameWorld()->GetPlayer()->GetTransform().GetWorldPosition() - 
		this->GetTransform().GetWorldPosition()).Length();

	StateFunc IdleFunc = [](void* data) {
		ParkKeeper* p = (ParkKeeper*)data;
		p->distance = (p->GetGameWorld()->GetPlayer()->GetTransform().GetWorldPosition() -
			p->GetTransform().GetWorldPosition()).Length();
		std::cout << "In State Idle!" << std::endl;
	};

	StateFunc ChaseFunc = [](void* data) {
		ParkKeeper* p = (ParkKeeper*)data;
		p->distance = (p->GetGameWorld()->GetPlayer()->GetTransform().GetWorldPosition() -
			p->GetTransform().GetWorldPosition()).Length();
		/*p->deltaTime += (p->timer->GetTimeDeltaSeconds()*10000);
		if (p->deltaTime > 1) {
			p->deltaTime = 0;
			p->pathFinding();
			p->nowDes = p->pathNodes->begin();
		}*/
		float movement = (p->GetGameWorld()->GetPlayer()->GetTransform().GetWorldPosition() - p->playerPrePos).Length();
		if (p->firstTime) {
			p->pathFinding();
			p->nowDes = p->pathNodes->begin();
			p->firstTime = false;
		}
		if (movement > 8) {
			p->SetPlayerPrePos();
			p->pathFinding();
			p->nowDes = p->pathNodes->begin();
		}
		p->Display();
		p->ChasePlayer();
		std::cout << "In State Chase!" << std::endl;
	};

	GenericState* idleState = new GenericState(IdleFunc, (void*)this);
	GenericState* chaseState = new GenericState(ChaseFunc, (void*)this);
	keeperStateMachine->AddState(idleState);
	keeperStateMachine->AddState(chaseState);

	GenericTransition<float&, float>* transitionA =
		new GenericTransition <float&, float>(GenericTransition<float&, float>::LessThanTransition, distance, 50, idleState, chaseState);
	GenericTransition<float&, float>* transitionB =
		new GenericTransition <float&, float>(GenericTransition<float&, float>::GreaterThanTransition, distance, 80, chaseState, idleState);

	keeperStateMachine->AddTransition(transitionA);
	keeperStateMachine->AddTransition(transitionB);
}
void ParkKeeper::Display() {
	for (int i = 1; i < pathNodes->size(); ++i) {
		Vector3 a = pathNodes->at(i - 1);
		Vector3 b = pathNodes->at(i);

		a.x = -(a.x - 100.0);
		a.y = 3;
		a.z = -(a.z - 80.0);
		b.x = -(b.x - 100.0);
		b.y = 3;
		b.z = -(b.z - 80.0);
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}
void ParkKeeper::pathFinding() {
	delete pathNodes;
	NavigationGrid grid("grid.txt");
	pathNodes = new vector<Vector3>;

	NavigationPath outPath;

	Vector3 startPos = this->GetTransform().GetWorldPosition();
	Vector3 endPos = this->GetGameWorld()->GetPlayer()->GetTransform().GetWorldPosition();
	startPos.x = 100.0 - startPos.x;
	startPos.z = 80.0 - startPos.z;
	//startPos.y = 0;
	endPos.x = 100.0 - endPos.x;
	endPos.z = 80.0 - endPos.z;
	//endPos.y = 0;

	bool found = grid.FindPath(startPos, endPos, outPath);
	std::cout << found << std::endl;

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		pathNodes->push_back(pos);
	}
}

void ParkKeeper::ChasePlayer() {
	Vector3 keeperPos = this->GetTransform().GetWorldPosition();
	keeperPos.y = 0;
	Vector3 desPos = *nowDes;

	desPos.x = 100 - desPos.x;
	desPos.z = 80 - desPos.z;
	desPos.y = 0;

	Vector3 distance = desPos - keeperPos;

	if (distance.Length() < 2) {
		if (nowDes < pathNodes->end()) {
			nowDes++;
			if (nowDes == pathNodes->end()) {
				//
			}
		}
	}
	else {
		distance.Normalise();
		this->GetPhysicsObject()->AddForce(distance * 30);
	}
}


