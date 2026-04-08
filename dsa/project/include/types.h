#pragma once
#include <array>
#include <cstddef>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

constexpr size_t NORMAL_QUEUE_CAPACITY = 50;
constexpr size_t UNDO_STACK_DEPTH = 10;
constexpr int MAX_URGENCY = 10;

struct Patient {
  int id;
  std::string name;
  int age;
};

struct PatientVisit {
  int patient_id;
  std::string condition;
  int urgency_level; // 0 = normal, 1–10 = emergency (higher urgency = higher
                     // priority)
  bool is_attended;
  std::string date;
  PatientVisit *next = nullptr;
};

struct AVLNode {
  Patient data;
  int height = 1;
  AVLNode *left = nullptr;
  AVLNode *right = nullptr;
};

struct NormalQueue {
  std::array<PatientVisit, NORMAL_QUEUE_CAPACITY> slots{};
  std::size_t head = 0;
  std::size_t tail = 0;
  std::size_t count = 0;
};

enum class OpType { REGISTER, ENQUEUE, UPDATE_PATIENT, UPDATE_VISIT };

struct UndoOp {
  OpType type;
  Patient patient_snapshot;    // used for REGISTER, UPDATE_PATIENT
  PatientVisit visit_snapshot; // used for ENQUEUE, UPDATE_VISIT
  int queue_target; // 0 = normal, 1 = emergency (used for ENQUEUE undo)
};

struct UndoStack {
  std::array<UndoOp, UNDO_STACK_DEPTH> ops{};
  int top = -1;
};

struct EmergencyComparator {
  bool operator()(const PatientVisit &a, const PatientVisit &b) const {
    return a.urgency_level < b.urgency_level;
  }
};

using EmergencyQueue =
    std::priority_queue<PatientVisit, std::vector<PatientVisit>,
                        EmergencyComparator>;

using HistoryMap = std::unordered_map<int, PatientVisit *>;

struct AppState {
  AVLNode *tree = nullptr;
  NormalQueue nqueue{};
  EmergencyQueue equeue{};
  HistoryMap history{};
  UndoStack undo{};
  int next_id = 1;
};
