#pragma once
#include "GameObject.h"
namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class PlayerGoose :public GameObject {
		public:
			PlayerGoose(string name = "");
			~PlayerGoose();
			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherobject) override;
		};
	}
}
