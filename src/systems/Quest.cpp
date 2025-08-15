#include "Quest.h"
#include <algorithm>

Quest Quest::createCollectItemQuest(const std::string& qid, const std::string& title, const std::string& itemId, int count) {
    Quest q(qid, title);
    Objective o;
    o.type = "collect_item";
    o.targetId = itemId;
    o.required = count;
    o.progress = 0;
    q.objectives.push_back(o);
    q.active = true;
    return q;
}

bool Quest::onItemCollected(const std::string& itemId) {
    bool changed = false;
    for (auto &o : objectives) {
        if (o.type == "collect_item" && o.targetId == itemId && !o.isComplete()) {
            ++o.progress;
            changed = true;
        }
    }
    // if all objectives complete, mark quest completed
    if (!objectives.empty() && std::all_of(objectives.begin(), objectives.end(), [](const Objective& o){ return o.isComplete(); })) {
        completed = true;
    }
    return changed;
}

void QuestManager::startQuest(Quest q) {
    if (!hasQuest(q.id)) quests.push_back(std::move(q));
}

void QuestManager::onItemCollected(const std::string& itemId) {
    for (auto &q : quests) {
        if (q.active && !q.completed) {
            if (q.onItemCollected(itemId)) {
                // could signal UI
            }
        }
    }
}

bool QuestManager::hasQuest(const std::string& id) const {
    for (auto &q : quests) if (q.id == id) return true;
    return false;
}

// global accessor
QuestManager& getGlobalQuestManager() {
    static QuestManager mgr;
    return mgr;
}
