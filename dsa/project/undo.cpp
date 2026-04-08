#include "types.h"
bool undo_empty(const UndoStack &s) { return s.top < 0; }

void undo_push(UndoStack &s, const UndoOp &op) {
  if (s.top == (int)(UNDO_STACK_DEPTH)-1) {
    // Shift left to drop oldest
    for (int i = 0; i < s.top; ++i)
      s.ops[i] = s.ops[i + 1];
    s.ops[s.top] = op;
  } else {
    s.ops[++s.top] = op;
  }
}

std::optional<UndoOp> undo_pop(UndoStack &s) {
  if (undo_empty(s))
    return std::nullopt;
  return s.ops[s.top--];
}
