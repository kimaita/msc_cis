#include "types.h"
#include "hospital.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

static std::string csv_escape(const std::string &s)
{
    if (s.find(',') == std::string::npos && s.find('"') == std::string::npos)
        return s;
    std::string out = "\"";
    for (char c : s)
    {
        if (c == '"')
            out += '"';
        out += c;
    }
    out += '"';
    return out;
}

static std::vector<std::string> csv_split(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;
    for (std::size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"')
        {
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                field += '"';
                ++i;
            }
            else
            {
                in_quotes = !in_quotes;
            }
        }
        else if (c == ',' && !in_quotes)
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

AVLNode *persistence_load_patients(const std::string &filepath)
{
    AVLNode *root = nullptr;
    std::ifstream f(filepath);
    if (!f.is_open())
        return root;
    std::string line;
    std::getline(f, line); // skip header
    while (std::getline(f, line))
    {
        if (line.empty())
            continue;
        auto fields = csv_split(line);
        if (fields.size() < 3)
            continue;
        Patient p;
        try
        {
            p.id = std::stoi(fields[0]);
            p.name = fields[1];
            p.age = std::stoi(fields[2]);
        }
        catch (...)
        {
            continue;
        }
        root = avl_insert(root, p);
    }
    return root;
}

static void collect_patients(AVLNode *root, std::vector<Patient> &out)
{
    if (!root)
        return;
    collect_patients(root->left, out);
    out.push_back(root->data);
    collect_patients(root->right, out);
}

void persistence_save_patients(AVLNode *root, const std::string &filepath)
{
    std::ofstream f(filepath);
    if (!f.is_open())
    {
        std::cerr << "Error: cannot write " << filepath << "\n";
        return;
    }
    f << "id,name,age\n";
    std::vector<Patient> patients;
    collect_patients(root, patients);
    for (const auto &p : patients)
    {
        f << p.id << "," << csv_escape(p.name) << "," << p.age << "\n";
    }
}

void persistence_load_visits(HistoryMap &hmap, const std::string &filepath)
{
    std::ifstream f(filepath);
    if (!f.is_open())
        return;
    std::string line;
    std::getline(f, line); // skip header

    struct Row
    {
        int id;
        std::string condition;
        int urgency;
        bool attended;
        std::string date;
    };
    std::vector<Row> rows;

    while (std::getline(f, line))
    {
        if (line.empty())
            continue;
        auto fields = csv_split(line);
        if (fields.size() < 5)
            continue;
        try
        {
            rows.push_back({std::stoi(fields[0]),
                            fields[1],
                            std::stoi(fields[2]),
                            fields[3] == "1",
                            fields[4]});
        }
        catch (...)
        {
            continue;
        }
    }
    for (auto it = rows.rbegin(); it != rows.rend(); ++it)
    {
        history_add(hmap, it->id, it->condition, it->urgency, it->attended, it->date);
    }
}

void persistence_save_visits(const HistoryMap &hmap, const std::string &filepath)
{
    std::ofstream f(filepath);
    if (!f.is_open())
    {
        std::cerr << "Error: cannot write " << filepath << "\n";
        return;
    }
    f << "patient_id,condition,urgency_level,is_attended,date\n";
    for (const auto &[id, head] : hmap)
    {
        std::vector<PatientVisit *> visits;
        for (PatientVisit *v = head; v; v = v->next)
            visits.push_back(v);

        for (auto vit = visits.rbegin(); vit != visits.rend(); ++vit)
        {
            PatientVisit *v = *vit;
            f << v->patient_id << ","
              << csv_escape(v->condition) << ","
              << v->urgency_level << ","
              << (v->is_attended ? 1 : 0) << ","
              << csv_escape(v->date) << "\n";
        }
    }
}