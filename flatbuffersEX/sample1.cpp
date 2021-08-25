/*
 * Copyright 2015 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "monster_generated.h"  // Already includes "flatbuffers/flatbuffers.h".
#include "iostream"
using namespace MyGame::Sample;
using namespace std;

// Example how to use FlatBuffers to create and read binary buffers.

int main(int /*argc*/, const char * /*argv*/[]) {
	// Build up a serialized buffer algorithmically:
	flatbuffers::FlatBufferBuilder builder;

	// First, lets serialize some weapons for the Monster: A 'sword' and an 'axe'.
	auto weapon_one_name = builder.CreateString("Sword");
	short weapon_one_damage = 3;

	auto weapon_two_name = builder.CreateString("Axe");
	short weapon_two_damage = 5;

	// Use the `CreateWeapon` shortcut to create Weapons with all fields set.
	auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
	auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

	// Create a FlatBuffer's `vector` from the `std::vector`.
	std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
	weapons_vector.push_back(sword);
	weapons_vector.push_back(axe);
	auto weapons = builder.CreateVector(weapons_vector);

	// Second, serialize the rest of the objects needed by the Monster.
	auto position = Vec3(1.0f, 2.0f, 3.0f);

	auto name = builder.CreateString("MyMonster");

	unsigned char inv_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	auto inventory = builder.CreateVector(inv_data, 10);

	// Shortcut for creating monster with all fields set:
	// 오크의 모든 구성요소 직렬화되었으므로 이제 오크 직렬화 함
	auto orc = CreateMonster(builder, &position, 150, 80, name, inventory,
		Color_Red, weapons, Equipment_Weapon, axe.Union());

	builder.Finish(orc);  // 객체의 root를 직렬화한다.
	//-----------------------------------------------------------------------
	//flatbuffers 데이터 받기
	// We now have a FlatBuffer we can store on disk or send over a network.

	// ** file/network code goes here :) **
	// access builder.GetBufferPointer() for builder.GetSize() bytes

	// Instead, we're going to access it right away (as if we just received it).

	// Get access to the root:
	// 몬스터라는 변수에 빌드된 데이터를 받아서 불러옴
	// builder.GetBufferPointer() 직렬화된 버퍼 가져옴
	auto monster = GetMonster(builder.GetBufferPointer()); //역직렬화

	cout << "몬스터 체력 : " << monster->hp() << std::endl;
	cout << "몬스터 마나 : " << monster->mana() << std::endl;
	cout << "몬스터 이름 : " << monster->name()->str() << std::endl;
	cout << "몬스터 위치 : " << monster->pos()->x() << ", " << monster->pos()->y() << ", " << monster->pos()->z() << std::endl;


//출처: https://windowshyun.tistory.com/23 

	// Get and test some scalar types from the FlatBuffer.
	assert(monster->hp() == 80);
	assert(monster->mana() == 150);  // default
	assert(monster->name()->str() == "MyMonster");

	// Get and test a field of the FlatBuffer's `struct`.
	auto pos = monster->pos();
	assert(pos);
	assert(pos->z() == 3.0f);
	(void)pos;

	// Get a test an element from the `inventory` FlatBuffer's `vector`.
	auto inv = monster->inventory();
	assert(inv);
	assert(inv->Get(9) == 9);
	(void)inv;

	// Get and test the `weapons` FlatBuffers's `vector`.
	std::string expected_weapon_names[] = { "Sword", "Axe" };
	short expected_weapon_damages[] = { 3, 5 };
	auto weps = monster->weapons();
	for (unsigned int i = 0; i < weps->size(); i++) {
		assert(weps->Get(i)->name()->str() == expected_weapon_names[i]);
		assert(weps->Get(i)->damage() == expected_weapon_damages[i]);
	}
	(void)expected_weapon_names;
	(void)expected_weapon_damages;

	// Get and test the `Equipment` union (`equipped` field).
	assert(monster->equipped_type() == Equipment_Weapon);
	auto equipped = static_cast<const Weapon *>(monster->equipped());
	assert(equipped->name()->str() == "Axe");
	assert(equipped->damage() == 5);
	(void)equipped;

	printf("The FlatBuffer was successfully created and verified!\n");
}