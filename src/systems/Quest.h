#pragma once
#include <string>
#include <vector>

struct Objective {
    std::string type; // e.g. "collect_item"
    std::string targetId; // e.g. item id
    int required = 1;
    int progress = 0;
    bool isComplete() const { return progress >= required; }
};

class Quest {
public:
    std::string id;
    std::string title;
    std::vector<Objective> objectives;
    bool active = false;
    bool completed = false;

    Quest() = default;
    Quest(std::string id_, std::string title_) : id(std::move(id_)), title(std::move(title_)) {}

    // Helper factory
    static Quest createCollectItemQuest(const std::string& qid, const std::string& title, const std::string& itemId, int count=1);

    // notify about item collection; returns true if quest state changed (objective progressed or completed)
    bool onItemCollected(const std::string& itemId);
};

class QuestManager {
public:
    void startQuest(Quest q);
    void onItemCollected(const std::string& itemId);
    const std::vector<Quest>& activeQuests() const { return quests; }

    bool hasQuest(const std::string& id) const;

private:
    std::vector<Quest> quests;
};
