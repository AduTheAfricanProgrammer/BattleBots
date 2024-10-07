extends Node



func get_player_name(id):
	if GameManager.player_info.has(id):
		return GameManager.player_info[id]["name"]
	else:
		return null
		

func get_player_id(id):
	if GameManager.player_info.has(id):
		return GameManager.player_info[id]["player_number"]
	else:
		return null

func get_in_singleplayer():
	return GameManager.is_in_singleplayer
	
func after_game_start():
	GameManager.after_game_start.rpc(GameManager.player_info[1]["name"])

func get_num_players():
	return GameManager.num_players

func get_already_started():
	return GameManager.game_already_started

func set_game_end(did_win):
	GameManager.end_state = did_win

func check_player_hurt():
	return GameManager.player_hurt
	
func set_player_hurt(update_player):
	GameManager.player_hurt = update_player

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
