#pragma once
#include "types.h"
#include <functional>

/**
 * Insert a patient into the AVL tree.
 * Returns new root.
 */
AVLNode *avl_insert(AVLNode *root, const Patient &p);

/**
 * Remove a node by patient_id.
 * Returns new root.
 */
AVLNode *avl_remove(AVLNode *root, int id);

/**
 * Search for a patient by id.
 * Returns pointer to node or nullptr.
 */
AVLNode *avl_search(AVLNode *root, int id);

/**
 * In-order traversal — calls visitor for each Patient.
 */
void avl_inorder(AVLNode *root, const std::function<void(const Patient &)> &visitor);

/**
 * Free all nodes in the tree.
 */
void avl_free(AVLNode *root);

/**
 * Enqueue a patient.
 * Returns false if queue is full.
 */
bool nq_enqueue(NormalQueue &q, const PatientVisit &v);

/**
 * Dequeue the front patient.
 * Returns nullopt if empty.
 */
std::optional<PatientVisit> nq_dequeue(NormalQueue &q);

/**
 * Peek at the front without removing.
 * Returns nullopt if empty.
 */
std::optional<Patient> nq_peek(const NormalQueue &q);

/**
 * Remove a specific patient by id.
 * Returns true if found and removed.
 */
bool nq_remove_by_id(NormalQueue &q, int id);

/**
 * Checks whether the normal patient queue is empty
 */
bool nq_empty(const NormalQueue &q);

/**
 * Checks whether the normal patient queue is full
 */
bool nq_full(const NormalQueue &q);

/**
 * Prints the normal patient queue
 */
void nq_display(const NormalQueue &q);

/**
 * Takes a patient from the emergency queue
 * Returns nullopt if queue is empty.
 */
std::optional<PatientVisit> eq_dequeue(EmergencyQueue &q);

/**
 * Remove a specific patient from the emergency queue by id.
 * Returns true if found.
 */
bool eq_remove_by_id(EmergencyQueue &q, int id);

/**
 * Prints the emergency patient queue
 */
void eq_display(EmergencyQueue q);

/** Returns 0 for normal, 1 for emergency based on urgency level. */
inline int triage_classify(const PatientVisit &p)
{
    return (p.urgency_level >= 1) ? 1 : 0;
}

/** Prepend a new visit to the patient's history list. */
void history_add(HistoryMap &hmap, int patient_id, const std::string &condition,
                 int urgency_level, bool is_attended, const std::string &date);

/** Prepend a pre-built PatientVisit (used by undo). */
void history_add_visit(HistoryMap &hmap, const PatientVisit &visit);

/**
 * Update the most recent visit for a patient.
 * Returns false if none exists.
 */
bool history_update_latest(HistoryMap &hmap, int patient_id,
                           const std::string &condition, int urgency_level, bool is_attended);

/** Display all visits for a patient in chronological order. */
void history_display(const HistoryMap &hmap, int patient_id);

/** Free all visit nodes for a patient. */
void history_free(HistoryMap &hmap, int patient_id);

/** Free all visit nodes in the map. */
void history_free_all(HistoryMap &hmap);

/** Push an operation. Drops oldest if at capacity. */
void undo_push(UndoStack &s, const UndoOp &op);

/** Pop the most recent operation. Returns nullopt if empty. */
std::optional<UndoOp> undo_pop(UndoStack &s);

bool undo_empty(const UndoStack &s);

/** Load patients from CSV into AVL tree. Returns new root. */
AVLNode *persistence_load_patients(const std::string &filepath);

/** Save all patients from AVL tree to CSV. */
void persistence_save_patients(AVLNode *root, const std::string &filepath);

/** Load visits from CSV into history map. */
void persistence_load_visits(HistoryMap &hmap, const std::string &filepath);

/** Save all visits from history map to CSV. */
void persistence_save_visits(const HistoryMap &hmap, const std::string &filepath);

/** Run the main console loop. Blocks until user exits. */
void ui_run(AppState &state);