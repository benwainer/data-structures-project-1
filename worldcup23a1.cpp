#include "worldcup23a1.h"
#include "cmath"
using namespace ds;
using namespace std;

#define WIN 3
#define DRAW 1

static void mergeArrays(Player* first_array, Player* second_array, Player* merged_array, int na, int nb, int nc)
{
    int ia, ib, ic;
    for(ia = ib = ic = 0; (ia < na) && (ib < nb); ic++)
    {
        if(first_array[ia] < second_array[ib]) {
            merged_array[ic] = first_array[ia];
            ia++;
        }
        else {
            merged_array[ic] = second_array[ib];
            ib++;
        }
    }
    for(;ia < na; ia++, ic++) merged_array[ic] = first_array[ia];
    for(;ib < nb; ib++, ic++) merged_array[ic] = second_array[ib];
}
world_cup_t::world_cup_t() : all_teams(Avl<Team>()),good_teams(Avl<Team>()),players_by_id(Avl<Player>()),players_by_goals(Avl<Player>()),
                             num_of_good_teams(0),num_of_players(0),  top_scorer(nullptr)
{}
world_cup_t::~world_cup_t()
{
    top_scorer= nullptr;
}
StatusType world_cup_t::add_team(int teamId, int points)
{
    if(teamId <= 0 || points < 0){
        return StatusType::INVALID_INPUT;
    }
    try {
        Team to_add(teamId,points);
        all_teams.insert(to_add);
        return StatusType::SUCCESS;
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    } catch(Avl<Team>::AlreadyExists& e){
        return StatusType::FAILURE;
    }
}

StatusType world_cup_t::remove_team(int teamId)
{
    if(teamId <= 0){
        return StatusType::INVALID_INPUT;
    }
    try{
        Team tmp_to_remove(teamId);
        Team* current_team = all_teams.find(tmp_to_remove);
        if(current_team->getNumOfPlayers() > 0){
            return StatusType::FAILURE;
        }
        else {
            all_teams.remove(*current_team);
        }
        return StatusType::SUCCESS;
    }
    catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(Avl<Team>::DoesNotExist& e){
        return StatusType::FAILURE;
    }
}

StatusType world_cup_t::add_player(int playerId, int teamId, int gamesPlayed,
                                   int goals, int cards, bool goalKeeper)
{
    if(playerId<=0 || teamId<=0 || gamesPlayed<0 || goals<0 || cards<0 || (gamesPlayed==0 && (goals>0 || cards>0)))
    {
        return StatusType::INVALID_INPUT;
    }
    try {
        Team tmp_players_team(teamId);
        Team *players_team = all_teams.find(tmp_players_team);
        return addPlayerAux(playerId,players_team,gamesPlayed,goals,cards,goalKeeper,teamId);
    }
    catch(std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(...){
        return StatusType::FAILURE;
    }


}
StatusType world_cup_t:: addPlayerAux(int playerId, Team* players_team, int gamesPlayed,
                                      int goals, int cards, bool goalKeeper,int teamId)
{
    try{
        int num = players_team->getNumOfMatches();
        Player player_by_goals(playerId,teamId,gamesPlayed,goals,cards,goalKeeper,players_team,num,true);
        player_by_goals.findandUpdateClosestPlayer(&players_by_goals);
        Player player_by_id(player_by_goals);
        player_by_id.changeByGoalsToById();
        players_by_id.insert(player_by_id);
        players_by_goals.insert(player_by_goals);
        player_by_goals.updateClosestPlayerForOthers(&players_by_goals, &players_by_id);
        top_scorer=players_by_goals.getMaxData();
        num_of_players++;
        Player* himself=players_by_id.find(player_by_id);
        player_by_id.updatePointersToHimself(himself);
        player_by_goals.updatePointersToHimself(himself);

        if(players_team->isGoodTeam()) {
            players_team->addPlayerToTeam(&player_by_id,&player_by_goals);
            return StatusType::SUCCESS;
        }

        players_team->addPlayerToTeam(&player_by_id,&player_by_goals);
        if(players_team->isGoodTeam())
        {
            Team good_team(teamId,players_team);
            good_teams.insert(good_team);
            num_of_good_teams++;
        }
        return StatusType::SUCCESS;
    }
    catch(std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(...){
        return StatusType::FAILURE;
    }

}

StatusType world_cup_t::remove_player(int playerId)
{
    if(playerId<=0)
    {
        return StatusType::INVALID_INPUT;
    }
    try {
        Player tmp_player(playerId);
        Player *player_to_remove_by_id = players_by_id.find(tmp_player);
        Player player_to_remove_by_goals(*player_to_remove_by_id);
        player_to_remove_by_goals.changeByIdToByGoals();
        Team *player_team = player_to_remove_by_id->getPlayersTeam();
        bool good_team = player_team->isGoodTeam();
        player_team->removePlayerFromTeam(*player_to_remove_by_id,player_to_remove_by_goals);
        if(!(player_team->isGoodTeam()) && good_team){
            good_teams.remove(*player_team);
            num_of_good_teams--;
        }
        players_by_id.remove(*player_to_remove_by_id);
        players_by_goals.remove(player_to_remove_by_goals);
        player_to_remove_by_goals.updateClosestPlayerForOthers(&players_by_goals,&players_by_id);
        top_scorer = players_by_goals.getMaxData();
        num_of_players--;
        return StatusType::SUCCESS;
    }
    catch(std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(ds::Avl<Player>::DoesNotExist& e){
        return StatusType::FAILURE;
    }

    // TODO: Your code goes here

}

StatusType world_cup_t::update_player_stats(int playerId, int gamesPlayed,
                                            int scoredGoals, int cardsReceived)
{
    if(playerId<=0 || gamesPlayed<0 || scoredGoals<0 || cardsReceived<0)
    {
        return StatusType::INVALID_INPUT;
    }
    try
    {
        Player tmp(playerId);
        Player* player_to_update=players_by_id.find(tmp);
        Team* players_team = player_to_update->getPlayersTeam();
        Player copy(*player_to_update);
        StatusType s=remove_player(playerId);
        if(s!=StatusType::SUCCESS)
        {
            return s;
        }
        if(gamesPlayed==0)
        {
            return addPlayerAux(playerId,players_team,copy.getNumPlayedGames(),scoredGoals,cardsReceived,
                                copy.isGoalkeeper(),players_team->getTeamId());
        }
        return addPlayerAux(playerId,players_team,copy.getNumPlayedGames()+gamesPlayed,
                            copy.getGoals()+scoredGoals,copy.getCards()+cardsReceived,
                            copy.isGoalkeeper(),players_team->getTeamId());

    }
    catch(std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(ds::Avl<Player>::DoesNotExist& e){
        return StatusType::FAILURE;
    }
    // TODO: Your code goes here
}

StatusType world_cup_t::play_match(int teamId1, int teamId2)
{
    if(teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2){
        return StatusType::INVALID_INPUT;
    }
    try{
        Team tmp_team1(teamId1);
        Team tmp_team2(teamId2);
        Team* team1 = all_teams.find(tmp_team1);
        Team* team2 = all_teams.find(tmp_team2);
        //Team* team1_in_good_teams = good_teams.find(tmp_team1);
        //Team* team2_in_good_teams = good_teams.find(tmp_team2);
        if(!(team1->isGoodTeam()) || !(team2->isGoodTeam())){
            return StatusType::FAILURE;
        }
        //the teams are OK
        int team1_value = team1->getTeamValue();
        int team2_value = team2->getTeamValue();
        if(team1_value > team2_value){ //team1 winner
            team1->addPoints(WIN);
            //team1_in_good_teams->addPoints(WIN);
        }
        else if(team1_value < team2_value) { //team2 winner
            team2->addPoints(WIN);
            //team2_in_good_teams->addPoints(WIN);
        }
        else { //
            team1->addPoints(DRAW);
            team2->addPoints(DRAW);
            //team1_in_good_teams->addPoints(DRAW);
            //team2_in_good_teams->addPoints(DRAW);
        }
        team1->addMatch();
        team2->addMatch();
        return StatusType::SUCCESS;
    }
    catch(bad_alloc& e){
        return StatusType::ALLOCATION_ERROR;
    }
    catch(Avl<Team>::DoesNotExist& e) {
        return StatusType::FAILURE;
    }
}

output_t<int> world_cup_t::get_num_played_games(int playerId)
{
    if(playerId <= 0){
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    try{
        Player tmp_by_id(playerId);
        Player* current_player = players_by_id.find(tmp_by_id);
        int num_played_games = current_player->getNumPlayedGamesCalculated(); //מחזירה מס' משחקים של שחקן ועוד מספר משחקים בקבוצתו פחות מספר המשחקים שהקבוצה שיחקה בלעדיו
        output_t<int> output1(num_played_games);
        return output1;
    }
    catch(bad_alloc& e) {
        output_t<int> output1(StatusType::ALLOCATION_ERROR);
        return output1;
    }
    catch(Avl<Player>::DoesNotExist& e){
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }

}

output_t<int> world_cup_t::get_team_points(int teamId)
{
    if(teamId <= 0){
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    try{
        Team tmp_team(teamId);
        Team* current_team = all_teams.find(tmp_team);
        int team_points = current_team->getTeamPoints();
        output_t<int> output1(team_points);
        return output1;
    }
    catch(bad_alloc& e) {
        output_t<int> output1(StatusType::ALLOCATION_ERROR);
        return output1;
    }
    catch(Avl<Team>::DoesNotExist& e){
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }
}

StatusType world_cup_t::unite_teams(int teamId1, int teamId2, int newTeamId)
{
    if(newTeamId <=0 || teamId1 <= 0 || teamId2 <= 0 || teamId2 == teamId1 ){
        return StatusType::INVALID_INPUT;
    }
    try{
        Team tmp_team1(teamId1);
        Team tmp_team2(teamId2);
        Team* team1_ptr = all_teams.find(tmp_team1);
        Team* team2_ptr = all_teams.find(tmp_team2);
        int team1_num = team1_ptr->getNumOfPlayers();
        int team2_num = team2_ptr->getNumOfPlayers();
        int total_num_of_players = team1_num+team2_num;
        Player team1_array_id[team1_num];
        Player team2_array_id[team2_num];
        Player merged_teams_id[total_num_of_players];
        Player team1_array_goals[team1_num];
        Player team2_array_goals[team2_num];
        Player merged_teams_goals[total_num_of_players];

        team1_ptr->insertPlayersToArrayId(team1_array_id);
        team1_ptr->insertPlayersToArrayGoals(team1_array_goals);
        team2_ptr->insertPlayersToArrayId(team2_array_id);
        team2_ptr->insertPlayersToArrayGoals(team2_array_goals);

        mergeArrays(team1_array_id,team2_array_id,merged_teams_id,team1_num,team2_num,total_num_of_players);
        mergeArrays(team1_array_goals,team2_array_goals,merged_teams_goals,team1_num,team2_num,total_num_of_players);
        //sizes for the united tree
        int h = ceil(log2(total_num_of_players + 1)) - 1;
        int n = pow(2, h) - 1;

        Player* ptr_id = NULL;
        Player* ptr_goals = NULL;
        if(newTeamId == teamId1){
            bool good_team_before = team1_ptr->isGoodTeam();
            //set team1 for the players in team 2
            for(int i=0; i<team2_num; i++) {
                ptr_id = &(team2_array_id[i]);
                ptr_id->setPlayersTeam(team1_ptr);
                ptr_id->addGamesOfTeamWithoutPlayer(team1_ptr->getNumOfMatches());
                ptr_goals = &(team2_array_goals[i]);
                ptr_goals->setPlayersTeamByGoals(team1_ptr);
                ptr_goals->addGamesOfTeamWithoutPlayer(team1_ptr->getNumOfMatches());
            }
            //set values for united team
            team1_ptr->setUniteTeamsValues(team2_ptr);
            if(team2_ptr->isGoodTeam()){
                good_teams.remove(*team2_ptr);
                num_of_good_teams--;
            }
            all_teams.remove(*team2_ptr);
            team1_ptr->insertPlayersToTreeId(merged_teams_id, h, n);
            team1_ptr->insertPlayersToTreeGoals(merged_teams_goals, h, n);
            team1_ptr->setNewTopScorer();
            if(!good_team_before){
                if(team1_ptr->isGoodTeam()){
                    Team copy(teamId1,team1_ptr);
                    good_teams.insert(copy);
                    num_of_good_teams++;
                }
            }
        }
        else if(newTeamId == teamId2){
            bool good_team_before = team2_ptr->isGoodTeam();
            //set team2 for the players in team 1
            for(int i=0; i<team1_num; i++) {
                ptr_id = &(team1_array_id[i]);
                ptr_id->setPlayersTeam(team2_ptr);
                ptr_id->addGamesOfTeamWithoutPlayer(team2_ptr->getNumOfMatches());
                ptr_goals = &(team1_array_goals[i]);
                ptr_goals->setPlayersTeamByGoals(team2_ptr);
                ptr_goals->addGamesOfTeamWithoutPlayer(team2_ptr->getNumOfMatches());
            }
            //set values for united team
            team2_ptr->setUniteTeamsValues(team1_ptr);
            if(team1_ptr->isGoodTeam()){
                good_teams.remove(*team1_ptr);
                num_of_good_teams--;
            }
            all_teams.remove(*team1_ptr);
            team2_ptr->insertPlayersToTreeId(merged_teams_id, h, n);
            team2_ptr->insertPlayersToTreeGoals(merged_teams_goals, h, n);
            team2_ptr->setNewTopScorer();
            if(!good_team_before){
                if(team2_ptr->isGoodTeam()){
                    Team copy(teamId2,team2_ptr);
                    good_teams.insert(copy);
                    num_of_good_teams++;
                }
            }
        }
        else {
            Team new_team(*team1_ptr, newTeamId);
            all_teams.insert(new_team); //if exist team with this teamId - throws exception
            Team* new_team_ptr = all_teams.find(new_team);
            for(int i=0; i<total_num_of_players; i++) {
                ptr_id = &(merged_teams_id[i]);
                if (ptr_id->getPlayersTeam() == team1_ptr) {
                    ptr_id->addGamesOfTeamWithoutPlayer(team2_ptr->getNumOfMatches());
                }
                else {
                    ptr_id->addGamesOfTeamWithoutPlayer(team1_ptr->getNumOfMatches());
                }
                ptr_id->setPlayersTeam(new_team_ptr);
                ptr_goals = &(merged_teams_goals[i]);
                if (ptr_goals->getPlayersTeam() == team1_ptr)
                    ptr_goals->addGamesOfTeamWithoutPlayer(team2_ptr->getNumOfMatches());
                else
                    ptr_goals->addGamesOfTeamWithoutPlayer(team1_ptr->getNumOfMatches());
                ptr_goals->setPlayersTeamByGoals(new_team_ptr);
            }
            new_team_ptr->setUniteTeamsValues(team2_ptr);
            new_team_ptr->insertPlayersToTreeId(merged_teams_id, h, n);
            new_team_ptr->insertPlayersToTreeGoals(merged_teams_goals, h, n);
            new_team_ptr->setNewTopScorer();
            if(team1_ptr->isGoodTeam()){
                good_teams.remove(*team1_ptr);
                num_of_good_teams--;
            }
            all_teams.remove(*team1_ptr);
            if(team2_ptr->isGoodTeam()){
                good_teams.remove(*team2_ptr);
                num_of_good_teams--;
            }
            all_teams.remove(*team2_ptr);
            if(new_team_ptr->isGoodTeam()){
                Team copy(newTeamId,new_team_ptr);
                good_teams.insert(copy);
                num_of_good_teams++;
            }
        }
    } catch(std::bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(...){
        return StatusType::FAILURE;
    }
    return StatusType::SUCCESS;
}

output_t<int> world_cup_t::get_top_scorer(int teamId)
{
    if(teamId == 0){
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    try{
        if(teamId < 0){
            if(num_of_players == 0){
                output_t<int> output1(StatusType::FAILURE);
                return output1;
            }
            else{
                //assume != NULL
                int top_scorer_id = top_scorer->getPlayerId();
                output_t<int> output1(top_scorer_id);
                return output1;
            }
        }
        else {
            Team tmp_team(teamId);
            Team* current_team = all_teams.find(tmp_team);
            if(current_team->getNumOfPlayers() == 0){
                output_t<int> output1(StatusType::FAILURE);
                return output1;
            }
            Player* top_scorer_in_team = current_team->getTopScorerPlayer();
            //assume != NULL
            int top_scorer_id = top_scorer_in_team->getPlayerId();
            output_t<int> output1(top_scorer_id);
            return output1;
        }
    } catch(bad_alloc& e) {
        output_t<int> output1(StatusType::ALLOCATION_ERROR);
        return output1;
    }
    catch(Avl<Team>::DoesNotExist& e) {
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }
}

output_t<int> world_cup_t::get_all_players_count(int teamId)
{
    if(teamId == 0){
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    if(teamId < 0){
        output_t<int> output1(num_of_players);
        return output1;
    }
    try{
        Team tmp_team(teamId);
        Team* current_team = all_teams.find(tmp_team);
        output_t<int> output1(current_team->getNumOfPlayers());
        return output1;
    }
    catch(bad_alloc& e) {
        output_t<int> output1(StatusType::ALLOCATION_ERROR);
        return output1;
    }
    catch(Avl<Team>::DoesNotExist& e) {
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }
}

StatusType world_cup_t::get_all_players(int teamId, int *const output)
{
    if(teamId == 0 || output == NULL){
        return StatusType::INVALID_INPUT;
    }
    try{
        Player* tmp_players_array = NULL;
        int array_length = 0;
        if(teamId < 0){
            array_length = num_of_players;
            if(array_length == 0){
                return StatusType::FAILURE;
            }
            tmp_players_array = new Player[array_length];
            int index = 0;
            players_by_goals.avlInsertToArrayInOrder(tmp_players_array,array_length,&index, players_by_goals.getRoot());
        }
        else {
            Team tmp_team(teamId);
            Team* current_team = all_teams.find(tmp_team);
            array_length = current_team->getNumOfPlayers();
            if(array_length == 0){
                return StatusType::FAILURE;
            }
            tmp_players_array = new Player[array_length];
            current_team->insertPlayersToArrayGoals(tmp_players_array);
        }
        for(int i=0; i<array_length; i++){
            output[i] = tmp_players_array[i].getPlayerId();
        }
        delete[] tmp_players_array;
        return StatusType::SUCCESS;
    } catch(bad_alloc& e) {
        return StatusType::ALLOCATION_ERROR;
    }
    catch(Avl<Team>::DoesNotExist& e) {
        return StatusType::FAILURE;
    }

}

output_t<int> world_cup_t::get_closest_player(int playerId, int teamId)
{
    if(playerId<=0 || teamId<=0)
    {
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    try {
        Team tmp(teamId);
        Team *players_team = all_teams.find(tmp);
        Player *player = players_team->findPlayerInTeam(playerId);
        int closest_player_id = player->getClosestPlayerId();
        if (closest_player_id == 0) {
            output_t<int> output1(StatusType::FAILURE);
            return output1;
        }
        output_t<int> output1(closest_player_id);
        return output1;
    }
    catch(std::bad_alloc&e) {
        output_t<int> output1(StatusType::ALLOCATION_ERROR);
        return output1;
    }
    catch(...) {
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }
}

output_t<int> world_cup_t::knockout_winner(int minTeamId, int maxTeamId)
{
    if(minTeamId<0 || maxTeamId<0 || maxTeamId<minTeamId)
    {
        output_t<int> output1(StatusType::INVALID_INPUT);
        return output1;
    }
    Team* teams_array = new Team[num_of_good_teams];
    Team tmp_min(minTeamId);
    Team tmp_max(maxTeamId);
    int index = 0;
    good_teams.makeArrayInBounds(teams_array,&index,num_of_good_teams,good_teams.getRoot(),tmp_min,tmp_max);
    //now index is r
    if(index == 0)
    {
        delete[] teams_array;
        output_t<int> output1(StatusType::FAILURE);
        return output1;
    }
    int* id_array = new int[index];
    int* score_array = new int[index];
    int r = index;
    for(int i=0;i<r;i++)
    {
        Team* team_ptr = teams_array[i].getTeamPointerInAllTeams();
        id_array[i]=team_ptr->getTeamId();
        score_array[i]=team_ptr->getTeamValue();
    }
    int side_team_id = 0;
    int side_team_score = 0;
    bool side_team = false;
    if(r%2 != 0){
        side_team = true;
        side_team_id = id_array[r-1];
        side_team_score = score_array[r-1];
    }
    int j = 0;
    while(r != 1)
    {
        j = 0;
        for (int i=0; i<r-1; i+=2)
        {
            if(score_array[i] > score_array[i+1] || (score_array[i] == score_array[i+1] && id_array[i] > id_array[i+1])){
                id_array[j] = id_array[i];
            }
            else if(score_array[i+1] > score_array[i] || (score_array[i] == score_array[i+1] && id_array[i+1] > id_array[i])){
                id_array[j] = id_array[i+1];
            }
            score_array[j] = score_array[i] + score_array[i+1] + WIN;
            j++;
        }
        r = r/2;
        if(r%2 != 0) {
            if (side_team) {
                id_array[j] = side_team_id;
                score_array[j] = side_team_score;
                r++;
                side_team = false;
            }
            else if(r!=1){
                side_team_id = id_array[r-1];
                side_team_score = score_array[r-1];
                side_team = true;
            }
        }
    }
    int winner_id = id_array[0];
    delete [] teams_array;
    delete [] id_array;
    delete [] score_array;
    output_t<int> output1(winner_id);
    return output1;

}

