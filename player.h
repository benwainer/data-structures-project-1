#ifndef PLAYER_H
#define PLAYER_H
#include <iostream>
#include <cstdio>
#include "Team.h"
#include "wet1util.h"
#include "worldcup23a1.h"
#include "Avl.h"
class Team;
class Player
{
private:
    int playerId;
    int teamId;
    Team* player_team;
    int goals;
    int cards;
    int games_played;
    int games_of_team_without_player;
    bool goalkeeper;
    Player* closest_player;
    bool checking_by_goals;
    Player* himself_in_players_by_id;
public:
    Player(int playerId);
    Player(int playerId, int teamId, int gamesPlayed, int goals, int cards, bool goalKeeper,Team* team,int games_of_team_without_player,bool checking_by_goals);
    Player();
    Player(const Player& player);//copy constructor
    ~Player();
    Player& operator=(const Player& player);
    bool operator==(const Player& player) const;
    bool operator!=(const Player& player) const;
    bool operator<(const Player& player) const;
    bool operator>(const Player& player) const;
    void updatePlayer(int playerId, int gamesPlayed, int scoredGoals, int cardsReceived);
    void updateClosestPlayer(Player* player);
    int getNumPlayedGamesCalculated() const;
    int getNumPlayedGames() const;
    Player* findtheClosest(Player* closest_from_top,Player* closest_from_bottom);
    void changeByIdToByGoals();
    void changeByGoalsToById();
    Team* getPlayersTeam() const;
    void updateClosestPlayerForOthers(ds::Avl<Player>* players_by_goals,ds::Avl<Player>* players_by_id);
    void updateHisFriends(ds::Avl<Player>* players_by_goals,ds::Avl<Player>* players_by_id,Player* closest_from_top,
                          Player* closest_from_bottom);
    void findandUpdateClosestPlayer(ds::Avl<Player>* players_by_goals);
    int getPlayerId() const;
    void updatePointersToHimself(Player* himself_in_players_by_id);
    int getGoals() const;
    int getCards() const;
    int getTeamId() const;
    bool isGoalkeeper() const;
    int getClosestPlayerId() const;
    void addGamesOfTeamWithoutPlayer(int num);
    void setPlayersTeam(Team* team_ptr);
    void setPlayersTeamByGoals(Team* team_ptr);


};


#endif
