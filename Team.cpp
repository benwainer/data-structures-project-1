
#include "Team.h"

using namespace ds;

Team::Team() : team_id(-1), players_by_id(nullptr), players_by_goals(nullptr) ,num_of_players(0),
points(0), goals(0), cards(0), num_of_matches(0), top_scorer(NULL), num_of_goal_keepers(0),
team_in_all_teams_tree(nullptr){}

Team::Team(int team_id) : team_id(team_id), players_by_id(new Avl<Player>), players_by_goals(new Avl<Player>) ,num_of_players(0),
points(0), goals(0), cards(0), num_of_matches(0), top_scorer(NULL), num_of_goal_keepers(0),team_in_all_teams_tree(nullptr) {}

Team::Team(int team_id, int points) : team_id(team_id), players_by_id(new Avl<Player>), players_by_goals(new Avl<Player>),
num_of_players(0), points(points), goals(0), cards(0), num_of_matches(0), top_scorer(NULL),
                                   num_of_goal_keepers(0), team_in_all_teams_tree(nullptr)
{}
Team::Team(int team_id, Team* team_ptr) : team_id(team_id), players_by_id(nullptr),
players_by_goals(nullptr) ,num_of_players(0), points(0), goals(0), cards(0), num_of_matches(0), top_scorer(NULL),
                                          num_of_goal_keepers(0){
    team_in_all_teams_tree = team_ptr;
                                          }


Team::Team(const Team &other) : team_id(other.team_id), players_by_id(new Avl<Player>), players_by_goals(new Avl<Player>),
num_of_players(other.num_of_players),  points(other.points),  goals(other.goals), cards(other.cards),
num_of_matches(other.num_of_matches), num_of_goal_keepers(other.num_of_goal_keepers)
{
    top_scorer = other.top_scorer;
    team_in_all_teams_tree = other.team_in_all_teams_tree;
}
Team::Team(const Team &other, int new_teamId) :
team_id(new_teamId), players_by_id(new Avl<Player>), players_by_goals(new Avl<Player>),
num_of_players(other.num_of_players),  points(other.points),  goals(other.goals), cards(other.cards),
num_of_matches(other.num_of_matches), num_of_goal_keepers(other.num_of_goal_keepers)
{
    top_scorer = other.top_scorer;
    team_in_all_teams_tree = other.team_in_all_teams_tree;
}

Team &Team::operator=(const Team &other)
{
    if(&other == this){
        return *this;
    }
    if (players_by_id != nullptr) {
        delete players_by_id;
        players_by_id = nullptr;
    }
    if (players_by_goals != nullptr) {
        delete players_by_goals;
        players_by_goals = nullptr;
    }
    team_id = other.team_id;
    num_of_players = other.num_of_players;
    points = other.points;
    goals = other.goals;
    cards = other.cards;
    num_of_matches = other.num_of_matches;
    top_scorer = other.top_scorer;
    num_of_goal_keepers = other.num_of_goal_keepers;
    team_in_all_teams_tree = other.team_in_all_teams_tree;
    return *this;
}

Team::~Team()
{
    if(players_by_id){
        delete players_by_id;
        players_by_id = nullptr;
    }
    if(players_by_goals){
        delete players_by_goals;
        players_by_goals = nullptr;
    }
    team_id = -1;
    num_of_players = -1;
    top_scorer = NULL;
}

bool Team::operator<(const Team &other) const
{
    return team_id < other.team_id;
}
bool Team::operator>(const Team &other) const
{
    return team_id > other.team_id;
}
bool Team::operator==(const Team &other) const
{
    return team_id == other.team_id;
}
int Team::getNumOfPlayers() const
{
    return num_of_players;
}
int Team::getTeamId() const
{
    return team_id;
}


int Team::getNumOfMatches() const
{
    return num_of_matches;
}
int Team::getTeamPoints() const
{
    return points;
}
int Team::getTeamValue() const
{
    return points + goals - cards;
}
//Team* Team::getTeamPointer() const
//{
//    return team_in_all_teams_tree;
//}
Player* Team::findPlayerInTeam(int playerId) const
{
    Player tmp(playerId);
    Player* the_player = players_by_id->find(tmp);
    return the_player;
}
void Team::addMatch()
{
    num_of_matches++;
}
void Team::addPoints(int num) {
    points+=num;
}
void Team::setUniteTeamsValues(Team* team)
{
    points += team->points;
    goals += team->goals;
    cards += team->cards;
    num_of_goal_keepers += team->num_of_goal_keepers;
    num_of_matches += team->num_of_matches;
    num_of_players += team->num_of_players;
}
void Team::setNewTopScorer() {
    top_scorer = players_by_goals->getMaxData();
}
Player *Team::getTopScorerPlayer() const
{
    return top_scorer;
}
Team *Team::getTeamPointerInAllTeams() const
{
    return team_in_all_teams_tree;
}
void Team::addPlayerToTeam(const Player* player_by_id, const Player* player_by_goals)
{
    players_by_id->insert(*player_by_id);
    players_by_goals->insert(*player_by_goals);
    if(player_by_id->isGoalkeeper()){
        num_of_goal_keepers++;
    }
    num_of_players++;
    goals+=player_by_id->getGoals();
    cards+=player_by_id->getCards();
    top_scorer = players_by_goals->getMaxData();
}
void Team::removePlayerFromTeam(const Player &player_by_id, const Player &player_by_goals)
{
    if(players_by_id == nullptr || players_by_id->getRoot() == NULL)
        return;
    if (players_by_goals == nullptr || players_by_goals->getRoot() == NULL)
        return;
    if(player_by_id.isGoalkeeper()){
        num_of_goal_keepers--;
    }
    players_by_id->remove(player_by_id);
    players_by_goals->remove(player_by_goals);
    num_of_players--;
    goals-=player_by_id.getGoals();
    cards-=player_by_id.getCards();
    if(num_of_players == 0){
        top_scorer = NULL;
    }
    else
        top_scorer = players_by_goals->getMaxData();
}

void Team::setClosestForPlayerInTeam(Player* player, Player* closet_to_him)
{
    Player* the_player = players_by_id->find(*player);
    the_player->updateClosestPlayer(closet_to_him);
}
bool Team::isGoodTeam() const {
    if(num_of_players >= 11 && num_of_goal_keepers > 0)
        return true;
    else
        return false;
}
void Team::insertPlayersToArrayId(Player *players_array_id)
{
    int index = 0;
    players_by_id->avlInsertToArrayInOrder(players_array_id, num_of_players, &index, players_by_id->getRoot());
}
void Team::insertPlayersToArrayGoals(Player *players_array_goals)
{
    int index = 0;
    players_by_goals->avlInsertToArrayInOrder(players_array_goals, num_of_players, &index, players_by_goals->getRoot());
}

void Team::insertPlayersToTreeId(Player *players_array_id, int height, int n)
{
    int index = 0;
    int num_of_players_copy = num_of_players;
    players_by_id->createEmptyTree(&num_of_players_copy, height,n);
    players_by_id->recursivePutValuesInOrder(players_array_id, players_by_id->getRoot(), num_of_players, &index);
}
void Team::insertPlayersToTreeGoals(Player *players_array_goals, int height, int n)
{
    int index = 0;
    int num_of_players_copy = num_of_players;
    players_by_goals->createEmptyTree(&num_of_players_copy, height,n);
    players_by_goals->recursivePutValuesInOrder(players_array_goals, players_by_goals->getRoot(), num_of_players, &index);
}










