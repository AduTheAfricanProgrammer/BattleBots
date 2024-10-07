extends Node


var player_info = {}
var is_in_singleplayer = false
var num_players = 0
var is_in_game = false
var game_already_started = false
var end_state = 0 # 0 for disconnect, 1 for loss, 2 for win
var player_hurt = 0 # 1 player 1 can be hurt, 2 player 2 can be hurt



# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
@rpc("any_peer", "call_remote", "reliable")
func update_player_info(id, name, player_num):
	player_info[id] = {
		"name": name,
		"player_number": player_num
	}
	num_players += 1

@rpc("any_peer", "call_remote", "reliable")
func after_game_start(p1_name):
	get_node("/root/GUI").call("after_game_start", p1_name)

func get_multiplayer_id_from_player_id(player_id):
	for multi_id in player_info:
		if player_info[multi_id].player_number == player_id:
			return multi_id
	return -1

func reset():
	player_info = {}
	is_in_singleplayer = false
	num_players = 0
	is_in_game = false
	game_already_started = false
	end_state = 0
