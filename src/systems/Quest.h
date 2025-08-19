#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct QuestObjective {
    std::string id; // e.g., "move_item_via_cart"
    bool completed = false;
    int progress = 0;
    int target = 1;
    nlohmann::json toJson() const { return { {"id",id},{"completed",completed},{"progress",progress},{"target",target} }; }
    static QuestObjective fromJson(const nlohmann::json& j) {
        QuestObjective o; if (j.contains("id")) o.id=j["id"].get<std::string>();
        o.completed = j.value("completed",false); o.progress=j.value("progress",0); o.target=j.value("target",1); return o; }
};

struct Quest {
    std::string id; // "starter_logistics"
    std::string title;
    std::string description;
    std::vector<QuestObjective> objectives;
    bool completed = false;
    nlohmann::json toJson() const {
        nlohmann::json oj = nlohmann::json::array(); for (auto &o: objectives) oj.push_back(o.toJson());
        return { {"id",id},{"title",title},{"description",description},{"completed",completed},{"objectives",oj} };
    }
    static Quest fromJson(const nlohmann::json& j) {
        Quest q; if (j.contains("id")) q.id=j["id"].get<std::string>(); q.title=j.value("title","Quest"); q.description=j.value("description",""); q.completed=j.value("completed",false); if (j.contains("objectives") && j["objectives"].is_array()) for (auto &o: j["objectives"]) q.objectives.push_back(QuestObjective::fromJson(o)); return q; }
};
