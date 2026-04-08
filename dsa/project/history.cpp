#include "types.h"
#include <iostream>
#include <iomanip>

void history_add(HistoryMap &hmap, int patient_id, const std::string &condition,
                 int urgency_level, bool is_attended, const std::string &date)
{
    auto *v = new PatientVisit{};
    v->patient_id = patient_id;
    v->condition = condition;
    v->urgency_level = urgency_level;
    v->is_attended = is_attended;
    v->date = date;
    v->next = hmap.count(patient_id) ? hmap[patient_id] : nullptr;
    hmap[patient_id] = v;
}

void history_add_visit(HistoryMap &hmap, const PatientVisit &visit)
{
    auto *v = new PatientVisit(visit);
    v->next = hmap.count(visit.patient_id) ? hmap[visit.patient_id] : nullptr;
    hmap[visit.patient_id] = v;
}

bool history_update_latest(HistoryMap &hmap, int patient_id,
                           const std::string &condition, int urgency_level, bool is_attended)
{
    auto it = hmap.find(patient_id);
    if (it == hmap.end() || !it->second)
        return false;
    it->second->condition = condition;
    it->second->urgency_level = urgency_level;
    it->second->is_attended = is_attended;
    return true;
}

void history_display(const HistoryMap &hmap, int patient_id)
{
    auto it = hmap.find(patient_id);
    if (it == hmap.end() || !it->second)
    {
        std::cout << "  No visit history found for patient " << patient_id << ".\n";
        return;
    }
    std::cout << "  Date        Urgency  Attended  Condition\n";
    std::cout << "  -----------------------------------------------\n";
    for (PatientVisit *v = it->second; v; v = v->next)
    {
        std::cout << "  " << std::setw(10) << v->date
                  << "  " << std::setw(7) << v->urgency_level
                  << "  " << std::setw(8) << std::left << (v->is_attended ? "Yes" : "No")
                  << "  " << v->condition << "\n"
                  << std::right;
    }
}

void history_free(HistoryMap &hmap, int patient_id)
{
    auto it = hmap.find(patient_id);
    if (it == hmap.end())
        return;
    PatientVisit *cur = it->second;
    while (cur)
    {
        PatientVisit *next = cur->next;
        delete cur;
        cur = next;
    }
    hmap.erase(it);
}

void history_free_all(HistoryMap &hmap)
{
    for (auto &[id, head] : hmap)
    {
        PatientVisit *cur = head;
        while (cur)
        {
            PatientVisit *next = cur->next;
            delete cur;
            cur = next;
        }
    }
    hmap.clear();
}