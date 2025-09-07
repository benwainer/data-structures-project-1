
#ifndef DATASTRUCTRESROUNDTWO_HW1_TEAM_H
#define DATASTRUCTRESROUNDTWO_HW1_TEAM_H
#include "Avl.h"
#include "player.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <memory>

class Player;
class Team {
private:
    int team_id;
    ds::Avl<Player>* players_by_id;
    ds::Avl<Player>* players_by_goals;
    int num_of_players;
    int points;
    int goals;
    int cards;
    int num_of_matches;
    Player *top_scorer;
    int num_of_goal_keepers;
    Team* team_in_all_teams_tree;
public:
    Team();
    Team(int team_id);
    Team(int team_id, int points);

    Team(int team_id, Team* team_ptr);
    Team(const Team &other);
    Team(const Team &other, int new_teamId);
    //Team(const Team &team1, const Team &team2, int new_teamId);
    Team &operator=(const Team &other);

    ~Team();

    bool operator<(const Team &other) const;

    bool operator>(const Team &other) const;

    bool operator==(const Team &other) const;

    int getNumOfPlayers() const;
    int getTeamId() const;
    int getTeamPoints() const;
    int getNumOfMatches() const;
    int getTeamValue() const;
    Player* findPlayerInTeam(int playerId) const;
    void addMatch();
    void addPoints(int num);
    void setUniteTeamsValues(Team* team);
    void setNewTopScorer();
    Player* getTopScorerPlayer() const;
    Team* getTeamPointerInAllTeams() const;
    void setClosestForPlayerInTeam(Player* player, Player* closet_to_him);
    void addPlayerToTeam(const Player *player_by_id, const Player* player_by_goals);
    void removePlayerFromTeam(const Player &player_by_id, const Player& player_by_goals);
    bool isGoodTeam() const;
    void insertPlayersToArrayId(Player* players_array_id);
    void insertPlayersToArrayGoals(Player* players_array_goals);
    void insertPlayersToTreeId(Player* players_array_id, int height, int n);
    void insertPlayersToTreeGoals(Player* players_array_goals, int height, int n);

};

#endif //DATASTRUCTRESROUNDTWO_HW1_TEAM_H
