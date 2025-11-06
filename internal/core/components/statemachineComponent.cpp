//
// Created by 张俏悦 on 2025/11/3.
//

#include "components.h"


class MovingState:public StateNode{
public:
    MovingState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::MOVING;
        stateMachine->manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType)
        ->MovingSignal.connect([this, stateMachine](b2Vec2 target) {
            stateMachine->SetStateNode(State::MOVING);
        });
    }

    void OnEnter() override {
        std::cout << "enter moving" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit moving" << std::endl;
        auto moving = stateMachine->manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
        b2BodyId body = b2LoadBodyId(moving->id);
        moving->target = b2Body_GetPosition(body);
    }

    void Update() override {
        b2Vec2 vec = b2Body_GetLinearVelocity(b2LoadBodyId(stateMachine->manager->id));
        if (vec.x == 0 && vec.y == 0) {
            stateMachine->SetStateNode(State::IDLE);
        }
    }

    void getMove(b2Vec2 target) {
        std::cout << "moving..." << std::endl;
    }
};


class IdleState:public StateNode {
public:
    IdleState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::IDLE;
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
        //  TODO 将攻击数据进行同步
    }
};


class SkillState:public StateNode {
public:
    SkillState(StateMachineComponent* stateMachine):StateNode(stateMachine) {
        state = State::SKILL;
    }

    void OnEnter() override {
        auto moving = stateMachine->manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
        moving->CanMove = false;
        b2BodyId body = b2LoadBodyId(moving->id);
        moving->target = b2Body_GetPosition(body);
        std::cout << "enter skill" << std::endl;
    }

    void OnExit() override {
        std::cout << "exit skill" << std::endl;
        auto moving = stateMachine->manager->GetComponent<MovingComponent>(ComponentType::MovingComponentType);
        moving->CanMove = true;
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
    SetStateNode(State::IDLE);
}
