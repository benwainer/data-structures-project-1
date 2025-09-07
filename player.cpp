#include "player.h"
#include <cmath>



Player:: Player(int playerId):
        playerId(playerId), teamId(-1), player_team(nullptr), goals(-1), cards(-1), games_played(0),
        games_of_team_without_player(-1), goalkeeper(false), closest_player(nullptr),checking_by_goals(false),
        himself_in_players_by_id(nullptr)
{}
Player:: Player(int playerId, int teamId, int gamesPlayed, int goals, int cards, bool goalKeeper,Team* team,int games_of_team_without_player,bool checking_by_goals):
        playerId(playerId),
        teamId(teamId),
        goals(goals),
        cards(cards),
        games_played(gamesPlayed),
        games_of_team_without_player(games_of_team_without_player),
        goalkeeper(goalKeeper),
        closest_player(nullptr),
        checking_by_goals(checking_by_goals),
        himself_in_players_by_id(nullptr)

{
    player_team = team;
}

Player:: Player() : playerId(-1), teamId(-1), player_team(nullptr), goals(-1), cards(-1), games_played(0),
                    games_of_team_without_player(-1), goalkeeper(false), closest_player(nullptr),checking_by_goals(false),
                    himself_in_players_by_id(nullptr)
{}

Player:: Player (const Player& player) :
        playerId(player.playerId), teamId(player.teamId), goals(player.goals), cards(player.cards), games_played(player.games_played),
        games_of_team_without_player(player.games_of_team_without_player),goalkeeper(player.goalkeeper), checking_by_goals(player.checking_by_goals)
        {
            closest_player = player.closest_player;
            player_team = player.player_team;
            himself_in_players_by_id = player.himself_in_players_by_id;
        }

Player:: ~Player()
{
    playerId = -1;
    teamId = -1;
    goals = -1;
    cards = -1;
    this->closest_player= nullptr;
    this->player_team= nullptr;
    this->himself_in_players_by_id= nullptr;
}
Player& Player::operator= (const Player& player)
{
    if(*this==player)
    {
        return *this;
    }
    this->playerId=player.playerId;
    this->teamId=player.teamId;
    this->player_team=player.player_team;
    this->goals=player.goals;
    this->cards=player.cards;
    this->games_played=player.games_played;
    this->goalkeeper=player.goalkeeper;
    this->closest_player=player.closest_player;
    this->checking_by_goals=player.checking_by_goals;
    this->himself_in_players_by_id=player.himself_in_players_by_id;
    return *this;
}
bool Player:: operator==(const Player& player) const
{
    return (this->playerId==player.playerId);
}
bool Player:: operator!=(const Player& player) const
{
    return (this->playerId!=player.playerId);
}
bool Player:: operator<(const Player& player) const
{
    if(this->checking_by_goals)
    {
        if(this->goals==player.goals)
        {
            if(this->cards==player.cards)
            {
                return this->playerId<player.playerId;
            }
            return this->cards>player.cards;
        }
        return this->goals<player.goals;
    }
    return this->playerId<player.playerId;
}

bool Player:: operator>(const Player& player) const
{

    if(this->checking_by_goals)
    {
        if(this->goals==player.goals)
        {
            if(this->cards==player.cards)
            {
                return this->playerId>player.playerId;
            }
            return this->cards<player.cards;
        }
        return this->goals>player.goals;
    }
    return this->playerId>player.playerId;

}
void Player:: updatePlayer(int playerId, int gamesPlayed, int scoredGoals, int cardsReceived)
{
    this->playerId=playerId;
    this->games_played=gamesPlayed;
    this->goals=scoredGoals;
    this->cards=cardsReceived;
}
void Player:: updateClosestPlayer(Player* player)
{
    this->closest_player=player;
}
int Player::getNumPlayedGamesCalculated() const
{
    return games_played + (player_team->getNumOfMatches()) - games_of_team_without_player;
}

int Player::getNumPlayedGames() const
{
    return games_played;
}
Player* Player:: findtheClosest(Player* closest_from_top,Player* closest_from_bottom)
{
    if(closest_from_top== nullptr && closest_from_bottom== nullptr)
    {
        return nullptr;
    }
    if(closest_from_bottom== nullptr)
    {
        return closest_from_top;
    }
    if(closest_from_top== nullptr)
    {
        return closest_from_bottom;
    }
    int goal_difference_from_top= abs(this->goals-closest_from_top->goals);
    int goal_difference_from_bottom= abs(this->goals-closest_from_bottom->goals);
    if(goal_difference_from_top==goal_difference_from_bottom)
    {
        int cards_difference_from_top= abs(this->cards-closest_from_top->cards);
        int cards_difference_from_bottom= abs(this->cards-closest_from_bottom->cards);
        if(cards_difference_from_top==cards_difference_from_bottom)
        {
            int playerId_difference_from_top= abs(this->playerId-closest_from_top->playerId);
            int playerId_difference_from_bottom= abs(this->playerId-closest_from_bottom->playerId);
            if(playerId_difference_from_top==playerId_difference_from_bottom)
            {
                if(closest_from_top->playerId>closest_from_bottom->playerId)
                {
                    return closest_from_top;
                }
                else
                {
                    return closest_from_bottom;
                }
            }
            if(playerId_difference_from_top<playerId_difference_from_bottom)
            {
                return closest_from_top;
            }
            else
            {
                return closest_from_bottom;
            }
        }
        if(cards_difference_from_top<cards_difference_from_bottom)
        {
            return closest_from_top;
        }
        else
        {
            return closest_from_bottom;
        }
    }
    if(goal_difference_from_top<goal_difference_from_bottom)
    {
        return closest_from_top;
    }
    else
    {
        return closest_from_bottom;
    }


}
void Player:: changeByIdToByGoals()
{
    checking_by_goals=true;
}
Team* Player:: getPlayersTeam() const
{
    return player_team;
}
void Player:: updateClosestPlayerForOthers(ds::Avl<Player>* players_by_goals,ds::Avl<Player>* players_by_id)
{
    Player* closest_from_top=players_by_goals->findNextInOrder(*this);
    Player* closest_from_bottom=players_by_goals->findPrevInOrder(*this);
    updateHisFriends(players_by_goals,players_by_id,closest_from_top,closest_from_bottom);

}

void Player:: findandUpdateClosestPlayer(ds::Avl<Player>* players_by_goals)
{
    Player* closest_player_from_top = players_by_goals->findNextInOrder(*this);
    Player* closest_player_from_bottom = players_by_goals->findPrevInOrder(*this);
    Player* closest_player = findtheClosest(closest_player_from_top, closest_player_from_bottom);
    updateClosestPlayer(closest_player);
}

int Player:: getPlayerId() const
{
    return playerId;
}
void Player:: updatePointersToHimself(Player* himself_in_players_by_id)
{
    this->himself_in_players_by_id=himself_in_players_by_id;

}
int Player:: getGoals() const
{
    return goals;
}
int Player:: getCards() const
{
    return cards;
}
void Player:: changeByGoalsToById()
{
    checking_by_goals=false;
}
int Player:: getTeamId() const
{
    return teamId;
}
bool Player:: isGoalkeeper() const
{
    return goalkeeper;
}

void Player:: updateHisFriends(ds::Avl<Player>* players_by_goals,ds::Avl<Player>* players_by_id,Player* closest_from_top,
                               Player* closest_from_bottom)
{
    if (closest_from_top != nullptr)
    {
        closest_from_top->findandUpdateClosestPlayer(players_by_goals);
        Player closest_from_top_by_id(*closest_from_top);
        closest_from_top_by_id.changeByGoalsToById();
        Player* closest_from_top_by_id_ptr=players_by_id->find(closest_from_top_by_id);
        closest_from_top_by_id_ptr->player_team->setClosestForPlayerInTeam(closest_from_top_by_id_ptr,closest_from_top->closest_player);
    }
    if (closest_from_bottom != nullptr) {
        closest_from_bottom->findandUpdateClosestPlayer(players_by_goals);
        Player closest_from_bottom_by_id(*closest_from_bottom);
        closest_from_bottom_by_id.changeByGoalsToById();
        Player* closest_from_bottom_by_id_ptr=players_by_id->find(closest_from_bottom_by_id);
        closest_from_bottom_by_id_ptr->player_team->setClosestForPlayerInTeam(closest_from_bottom_by_id_ptr,closest_from_bottom->closest_player);
    }

}
int Player:: getClosestPlayerId() const
{
    if(closest_player== nullptr)
    {
        return 0;
    }
    return closest_player->playerId;
}

void Player::addGamesOfTeamWithoutPlayer(int num) {
    games_of_team_without_player+=num;
}

void Player::setPlayersTeam(Team *team) {
    player_team = team;
    himself_in_players_by_id->player_team = team;
    teamId=team->getTeamId();
}
void Player::setPlayersTeamByGoals(Team *team) {
    player_team = team;
    teamId=team->getTeamId();
}

