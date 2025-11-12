//
// Created by 张俏悦 on 2025/11/3.
//

#include "components.h"


class MovingState:public StateNode{
public:
    MovingState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::MOVING;
    }

    void OnEnter() override {
        std::cout << "enter moving" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit moving" << std::endl;
        auto moving = stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType);
        b2BodyId body = b2LoadBodyId(moving->id);
        moving->target = b2Body_GetPosition(body);
    }

    void Update() override {
        b2Vec2 vec = b2Body_GetLinearVelocity(b2LoadBodyId(stateMachine->manager->id));
        if (vec.x == 0 && vec.y == 0) {
            stateMachine->SetStateNode(State::IDLE);
        }
    }
};


class IdleState:public StateNode {
public:
    IdleState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::IDLE;
        stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType)
        ->MovingSignal.connect([this, stateMachine](b2Vec2 target) {
            if (activate) {
                // 只有当处于静止状态时进行移动才会改变为移动状态
                stateMachine->SetStateNode(State::MOVING);
            }
        });
    }

    void OnEnter() override {
        std::cout << "enter idle" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit idle" << std::endl;
    }

    void Update() override {

    }
};


class AttackState:public StateNode {
public:
    AttackState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::ATTACK;
        stateMachine->manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType)
            ->startAttackSignal.connect([this, stateMachine](uint64_t target) {
                stateMachine->SetStateNode(State::ATTACK);
            });
        stateMachine->manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType)
            ->stopAttackSignal.connect([this,stateMachine](uint64_t target) {
               stateMachine->SetStateNode(State::IDLE);
            });
    }

    void OnEnter() override {
        std::cout << "enter attack" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit attack" << std::endl;
        auto attack = stateMachine->manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
        attack->isShooting = false;
    }

    void Update() override {

    }
};


class SkillState:public StateNode {
public:
    SkillState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::SKILL;
        stateMachine->manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType)->UseNormalSkill.connect(
            [this](Skill* skill) {
                currentSkill = skill;
                this->stateMachine->SetStateNode(State::SKILL);
            }
        );
    }

    void OnEnter() override {
        auto moving = stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType);
        moving->Enable = false;
        b2BodyId body = b2LoadBodyId(moving->id);
        moving->target = b2Body_GetPosition(body);
        std::cout << "enter skill" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit skill" << std::endl;
        auto moving = stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType);
        moving->Enable = true;
        currentSkill->Interrupt();
        currentSkill = nullptr;
    }

    void Update() override {

    }

    Skill* currentSkill;
};


class CCState:public StateNode {
public:
    CCState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::CC;
    }

    void OnEnter() override {
        std::cout << "enter cc" << std::endl;
        // 需要禁用移动组件和攻击组件以及技能组件
        auto moving = stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType);
        moving->Enable = false;
        auto attack = stateMachine->manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
        attack->Enable = false;
        auto skill = stateMachine->manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
        skill->Enable = false;
    }


    void OnExit() override {
        std::cout << "exit cc" << std::endl;
        auto moving = stateMachine->manager->GetComponent<MoveTargetComponent>(ComponentType::MoveTargetComponentType);
        moving->Enable = true;
        auto attack = stateMachine->manager->GetComponent<AttackComponent>(ComponentType::AttackComponentType);
        attack->Enable = true;
        auto skill = stateMachine->manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType);
        skill->Enable = true;
    }

    void Update() override {

    }
};


class MoveSkillState:public StateNode{
public:
    MoveSkillState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::MOVE_SKILL;
        stateMachine->manager->GetComponent<SkillComponent>(ComponentType::SkillComponentType)->UseNormalSkill.connect(
            [this](Skill* skill) {
                this->stateMachine->SetStateNode(State::MOVE_SKILL);
            }
        );
    }

    void OnEnter() override {
        std::cout << "enter move skill" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit move skill" << std::endl;
    }

    void Update() override {

    }
};


void StateMachineComponent::Update() {
    currentNode->Update();
}


void StateMachineComponent::InitializeNodes() {
    currentNode = nullptr;
    nodes.push_back(std::make_unique<IdleState>(this));
    nodes.push_back(std::make_unique<MovingState>(this));
    nodes.push_back(std::make_unique<AttackState>(this));
    nodes.push_back(std::make_unique<SkillState>(this));
    nodes.push_back(std::make_unique<CCState>(this));
    nodes.push_back(std::make_unique<MoveSkillState>(this));
    SetStateNode(State::IDLE);
}
