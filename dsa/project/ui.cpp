#include "hospital.h"
#include "types.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

static void clear_input() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string current_date() {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm *tm_info = std::localtime(&t);
  char buf[11];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
  return buf;
}

static void print_patient(const Patient &p) {
  std::cout << "  ID   : " << p.id << "\n"
            << "  Name : " << p.name << "\n"
            << "  Age  : " << p.age << "\n";
}

static void print_visit(const PatientVisit &v) {
  std::cout << "  Patient ID : " << v.patient_id << "\n"
            << "  Condition  : " << v.condition << "\n"
            << "  Urgency    : " << v.urgency_level
            << (v.urgency_level == 0 ? " (Normal)" : " (Emergency)") << "\n"
            << "  Attended   : " << (v.is_attended ? "Yes" : "No") << "\n"
            << "  Date       : " << v.date << "\n";
}

static void print_separator() {
  std::cout << "  " << std::string(50, '-') << "\n";
}

static int read_int(const std::string &prompt, int lo, int hi) {
  int val;
  while (true) {
    std::cout << prompt;
    if (std::cin >> val && val >= lo && val <= hi) {
      clear_input();
      return val;
    }
    std::cout << "  Invalid. Enter a number between " << lo << " and " << hi
              << ".\n";
    clear_input();
  }
}

static std::string read_line(const std::string &prompt) {
  std::cout << prompt;
  std::string s;
  std::getline(std::cin, s);
  return s;
}

// ---------------------------------------------------------------------------
// Operations
// ---------------------------------------------------------------------------

static void op_register(AppState &state) {
  std::cout << "\n  --- Register New Patient ---\n";
  Patient p;
  p.id = state.next_id;

  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  p.name = read_line("  Name : ");
  p.age = read_int("  Age  : ", 0, 150);

  state.tree = avl_insert(state.tree, p);
  ++state.next_id;

  undo_push(state.undo, {OpType::REGISTER, p, {}, 0});
  std::cout << "\n  Patient registered with ID " << p.id << ".\n";
}

static void op_add_visit(AppState &state) {
  std::cout << "\n  --- Add Visit for Patient ---\n";
  int id = read_int("  Patient ID: ", 1, std::numeric_limits<int>::max());

  AVLNode *node = avl_search(state.tree, id);
  if (!node) {
    std::cout << "  Patient not found.\n";
    return;
  }

  PatientVisit v{};
  v.patient_id = id;
  v.date = current_date();

  v.condition = read_line("  Condition : ");
  v.urgency_level =
      read_int("  Urgency (0=normal, 1-10=emergency): ", 0, MAX_URGENCY);
  v.is_attended = false;

  history_add(state.history, v.patient_id, v.condition, v.urgency_level,
              v.is_attended, v.date);

  int target = triage_classify(v);
  if (target == 0) {
    if (!nq_enqueue(state.nqueue, v)) {
      std::cout << "  Visit recorded but normal queue is full.\n";
      return;
    }
    std::cout << "  Visit recorded. Patient added to normal queue.\n";
  } else {
    state.equeue.push(v);
    std::cout << "  Visit recorded. Patient added to emergency queue (urgency "
              << v.urgency_level << ").\n";
  }

  undo_push(state.undo, {OpType::UPDATE_VISIT, {}, v, 0});
  undo_push(state.undo, {OpType::ENQUEUE, {}, v, target});
}

static void op_process(AppState &state) {
  std::cout << "\n  --- Process Next Patient ---\n";
  std::cout << "  1. Process from emergency queue\n"
            << "  2. Process from normal queue\n";
  int choice = read_int("  Choice: ", 1, 2);

  std::optional<PatientVisit> opt =
      (choice == 1) ? eq_dequeue(state.equeue) : nq_dequeue(state.nqueue);

  if (!opt) {
    std::cout << "  Queue is empty.\n";
    return;
  }

  PatientVisit v = *opt;
  v.is_attended = true;

  AVLNode *node = avl_search(state.tree, v.patient_id);
  std::cout << "\n  Processing patient:\n";
  if (node)
    print_patient(node->data);
  print_visit(v);

  history_update_latest(state.history, v.patient_id, v.condition,
                        v.urgency_level, true);
  std::cout << "  Patient marked as attended.\n";
}

static void op_update_patient(AppState &state) {
  std::cout << "\n  --- Update Patient Details ---\n";
  int id = read_int("  Patient ID: ", 1, std::numeric_limits<int>::max());

  AVLNode *node = avl_search(state.tree, id);
  if (!node) {
    std::cout << "  Patient not found.\n";
    return;
  }

  Patient old_snapshot = node->data;
  Patient updated = old_snapshot;

  std::cout << "  Leave blank to keep current value.\n";

  std::cout << "  Name [" << updated.name << "]: ";
  std::string input;
  std::getline(std::cin, input);
  if (!input.empty())
    updated.name = input;

  int new_age = read_int(
      "  Age [" + std::to_string(updated.age) + "] (-1 to skip): ", -1, 150);
  if (new_age >= 0)
    updated.age = new_age;

  undo_push(state.undo, {OpType::UPDATE_PATIENT, old_snapshot, {}, 0});
  state.tree = avl_insert(state.tree, updated);
  std::cout << "  Patient record updated.\n";
}

static void op_update_visit(AppState &state) {
  std::cout << "\n  --- Update Latest Visit ---\n";
  int id = read_int("  Patient ID: ", 1, std::numeric_limits<int>::max());

  auto it = state.history.find(id);
  if (it == state.history.end() || !it->second) {
    std::cout << "  No visit found for this patient.\n";
    return;
  }

  PatientVisit old_snapshot = *it->second;

  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cout << "  Leave blank to keep current value.\n";

  std::string new_condition;
  std::cout << "  Condition [" << old_snapshot.condition << "]: ";
  std::getline(std::cin, new_condition);
  if (new_condition.empty())
    new_condition = old_snapshot.condition;

  int new_urgency =
      read_int("  Urgency [" + std::to_string(old_snapshot.urgency_level) +
                   "] (-1 to skip): ",
               -1, MAX_URGENCY);
  if (new_urgency < 0)
    new_urgency = old_snapshot.urgency_level;

  undo_push(state.undo, {OpType::UPDATE_VISIT, {}, old_snapshot, 0});
  history_update_latest(state.history, id, new_condition, new_urgency,
                        old_snapshot.is_attended);
  std::cout << "  Visit updated.\n";
}

static void op_search(AppState &state) {
  std::cout << "\n  --- Search Patient ---\n";
  int id = read_int("  Patient ID: ", 1, std::numeric_limits<int>::max());

  AVLNode *node = avl_search(state.tree, id);
  if (!node) {
    std::cout << "  Patient not found.\n";
    return;
  }

  print_separator();
  print_patient(node->data);
  print_separator();
  std::cout << "  Visit History:\n";
  history_display(state.history, id);
}

static void op_list_all(AppState &state) {
  std::cout << "\n  --- All Registered Patients ---\n";
  bool any = false;
  avl_inorder(state.tree, [&](const Patient &p) {
    any = true;
    std::cout << "  [" << std::setw(4) << p.id << "]  " << std::setw(24)
              << std::left << p.name << "  Age: " << p.age << "\n"
              << std::right;
  });
  if (!any)
    std::cout << "  No patients registered.\n";
}

static void op_queues(AppState &state) {
  std::cout << "\n  Emergency Queue:\n";
  eq_display(state.equeue);
  std::cout << "\n  Normal Queue:\n";
  nq_display(state.nqueue);
}

static void op_undo(AppState &state) {
  auto opt = undo_pop(state.undo);
  if (!opt) {
    std::cout << "  Nothing to undo.\n";
    return;
  }

  const UndoOp &op = *opt;
  switch (op.type) {
  case OpType::REGISTER:
    state.tree = avl_remove(state.tree, op.patient_snapshot.id);
    if (state.next_id == op.patient_snapshot.id + 1)
      --state.next_id;
    std::cout << "  Undo: registration of patient " << op.patient_snapshot.id
              << " reversed.\n";
    break;

  case OpType::ENQUEUE:
    if (op.queue_target == 0)
      nq_remove_by_id(state.nqueue, op.visit_snapshot.patient_id);
    else
      eq_remove_by_id(state.equeue, op.visit_snapshot.patient_id);
    std::cout << "  Undo: enqueue of patient " << op.visit_snapshot.patient_id
              << " reversed.\n";
    break;

  case OpType::UPDATE_PATIENT:
    state.tree = avl_insert(state.tree, op.patient_snapshot);
    std::cout << "  Undo: patient update for ID " << op.patient_snapshot.id
              << " reversed.\n";
    break;

  case OpType::UPDATE_VISIT: {
    const PatientVisit &snap = op.visit_snapshot;
    history_update_latest(state.history, snap.patient_id, snap.condition,
                          snap.urgency_level, snap.is_attended);
    std::cout << "  Undo: visit update for patient " << snap.patient_id
              << " reversed.\n";
    break;
  }
  }
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------

static void print_menu() {
  std::cout << "\n";
  std::cout << "  ╔════════════════════════════════════════╗\n";
  std::cout << "  ║   Hospital Patient Management System   ║\n";
  std::cout << "  ╠════════════════════════════════════════╣\n";
  std::cout << "  ║  1.  Register new patient              ║\n";
  std::cout << "  ║  2.  Record patient visit              ║\n";
  std::cout << "  ║  3.  Process next patient              ║\n";
  std::cout << "  ║  4.  Update patient details            ║\n";
  std::cout << "  ║  5.  Update latest visit               ║\n";
  std::cout << "  ║  6.  Search patient by ID              ║\n";
  std::cout << "  ║  7.  List all patients                 ║\n";
  std::cout << "  ║  8.  View queues                       ║\n";
  std::cout << "  ║  9.  Undo last operation               ║\n";
  std::cout << "  ║  0.  Save & Exit                       ║\n";
  std::cout << "  ╚════════════════════════════════════════╝\n";
  std::cout << "  Choice: ";
}

void ui_run(AppState &state) {
  while (true) {
    print_menu();
    int choice;
    if (!(std::cin >> choice)) {
      clear_input();
      continue;
    }

    switch (choice) {
    case 1:
      op_register(state);
      break;
    case 2:
      op_add_visit(state);
      break;
    case 3:
      op_process(state);
      break;
    case 4:
      op_update_patient(state);
      break;
    case 5:
      op_update_visit(state);
      break;
    case 6:
      op_search(state);
      break;
    case 7:
      op_list_all(state);
      break;
    case 8:
      op_queues(state);
      break;
    case 9:
      op_undo(state);
      break;
    case 0:
      std::cout << "\n  Saving data...\n";
      return;
    default:
      std::cout << "  Invalid choice.\n";
    }
  }
}
