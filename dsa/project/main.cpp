#include "types.h"
#include "hospital.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

static const std::string PATIENTS_CSV = "data/patients.csv";
static const std::string VISITS_CSV = "data/visits.csv";

static int max_patient_id(AVLNode *root)
{
    if (!root)
        return 0;
    int id = root->data.id;
    return std::max({id, max_patient_id(root->left), max_patient_id(root->right)});
}

int main()
{
    std::filesystem::create_directories("data");

    AppState state;
    state.tree = persistence_load_patients(PATIENTS_CSV);
    state.next_id = max_patient_id(state.tree) + 1;
    persistence_load_visits(state.history, VISITS_CSV);

    std::cout << "  Loaded " << (state.next_id - 1) << " patient record(s).\n";

    ui_run(state);

    persistence_save_patients(state.tree, PATIENTS_CSV);
    persistence_save_visits(state.history, VISITS_CSV);

    avl_free(state.tree);
    history_free_all(state.history);

    std::cout << "  Data saved. Exiting...\n";
    return 0;
}