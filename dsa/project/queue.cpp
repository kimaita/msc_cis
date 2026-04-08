#include "types.h"

#include <iomanip>
#include <iostream>
#include <vector>

bool nq_empty(const NormalQueue &q) { return q.count == 0; }
bool nq_full(const NormalQueue &q) { return q.count == NORMAL_QUEUE_CAPACITY; }

bool nq_enqueue(NormalQueue &q, const PatientVisit &v) {
  if (nq_full(q))
    return false;
  q.slots[q.tail] = v;
  q.tail = (q.tail + 1) % NORMAL_QUEUE_CAPACITY;
  ++q.count;
  return true;
}

std::optional<PatientVisit> nq_dequeue(NormalQueue &q) {
  if (nq_empty(q))
    return std::nullopt;
  PatientVisit v = q.slots[q.head];
  q.head = (q.head + 1) % NORMAL_QUEUE_CAPACITY;
  --q.count;
  return v;
}

std::optional<PatientVisit> nq_peek(const NormalQueue &q) {
  if (nq_empty(q))
    return std::nullopt;
  return q.slots[q.head];
}

bool nq_remove_by_id(NormalQueue &q, int patient_id) {
  for (std::size_t i = 0; i < q.count; ++i) {
    std::size_t idx = (q.head + i) % NORMAL_QUEUE_CAPACITY;
    if (q.slots[idx].patient_id == patient_id) {
      for (std::size_t j = i; j < q.count - 1; ++j) {
        std::size_t cur = (q.head + j) % NORMAL_QUEUE_CAPACITY;
        std::size_t next = (q.head + j + 1) % NORMAL_QUEUE_CAPACITY;
        q.slots[cur] = q.slots[next];
      }
      q.tail = (q.tail == 0) ? NORMAL_QUEUE_CAPACITY - 1 : q.tail - 1;
      --q.count;
      return true;
    }
  }
  return false;
}

void nq_display(const NormalQueue &q) {
  if (nq_empty(q)) {
    std::cout << "  [Normal queue is empty]\n";
    return;
  }
  std::cout << "  Pos  PatientID  Date        Urgency  Attended  Condition\n";
  std::cout << "  ----------------------------------------------------------\n";
  for (std::size_t i = 0; i < q.count; ++i) {
    const auto &v = q.slots[(q.head + i) % NORMAL_QUEUE_CAPACITY];
    std::cout << "  " << std::setw(3) << (i + 1) << "  " << std::setw(9)
              << v.patient_id << "  " << std::setw(10) << v.date << "  "
              << std::setw(7) << v.urgency_level << "  " << std::setw(8)
              << std::left << (v.is_attended ? "Yes" : "No") << "  "
              << v.condition << "\n"
              << std::right;
  }
}

std::optional<PatientVisit> eq_dequeue(EmergencyQueue &q) {
  if (q.empty())
    return std::nullopt;
  PatientVisit v = q.top();
  q.pop();
  return v;
}

bool eq_remove_by_id(EmergencyQueue &q, int patient_id) {
  std::vector<PatientVisit> tmp;
  bool found = false;
  while (!q.empty()) {
    if (q.top().patient_id == patient_id) {
      found = true;
      q.pop();
      continue;
    }
    tmp.push_back(q.top());
    q.pop();
  }
  for (auto &v : tmp)
    q.push(v);
  return found;
}

void eq_display(EmergencyQueue q) {
  if (q.empty()) {
    std::cout << "  [Emergency queue is empty]\n";
    return;
  }
  std::cout << "  Pos  PatientID  Date        Urgency  Attended  Condition\n";
  std::cout << "  ----------------------------------------------------------\n";
  int pos = 1;
  while (!q.empty()) {
    const auto &v = q.top();
    std::cout << "  " << std::setw(3) << pos++ << "  " << std::setw(9)
              << v.patient_id << "  " << std::setw(10) << v.date << "  "
              << std::setw(7) << v.urgency_level << "  " << std::setw(8)
              << std::left << (v.is_attended ? "Yes" : "No") << "  "
              << v.condition << "\n"
              << std::right;
    q.pop();
  }
}
