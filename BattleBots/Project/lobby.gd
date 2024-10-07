extends Control

var is_ready = false # To track if the client is ready
var has_updated_text = false


	
# Called when the node enters the scene tree for the first time.
func _ready():
	multiplayer.peer_connected.connect(on_player_connected)
	multiplayer.peer_disconnected.connect(on_player_disconnected)
	multiplayer.connected_to_server.connect(on_connected_to_server)
	if(multiplayer.is_server()):
		get_node("ReadyButton").text = "START"
		get_node("PlayerBoxes/P1/Name").text = "cube 1"
		get_node("PlayerBoxes/P1/Ready").text = "✓"
		get_node("ReadyButton").disabled = false
		print("HI")
		GameManager.player_info[1] = {
			"name": "cube 1",
			"player_number": 1
		}
	else:
		get_node("ReadyButton").text = "READY"
		get_node("PlayerBoxes/P2/Name").text = "cube 2"
		GameManager.player_info[multiplayer.get_unique_id()] = {
			"name": "cube 2",
			"player_number": 2
		}
		
func on_player_connected(id):
	GameManager.num_players += 1
	# The server joined
	if(id != 1):
		get_node("PlayerBoxes/P2/Name").text = "cube 2"
		get_node("ReadyButton").disabled = true
		print(multiplayer.get_unique_id())
		update_text.rpc(GameManager.player_info[1]["name"], "PlayerBoxes/P1/Name", 1)
		update_player_info.rpc(multiplayer.get_unique_id(), GameManager.player_info[1]["name"], 1)
	# The other player joined
	# Called on the other player
	else:
		get_node("PlayerBoxes/P1/Name").text = "cube 1"
		get_node("PlayerBoxes/P1/Ready").text = "✓"
		get_node("ReadyButton").disabled = false
		update_text.rpc(GameManager.player_info[multiplayer.get_unique_id()]["name"], "PlayerBoxes/P2/Name", multiplayer.get_unique_id())
		update_player_info.rpc(multiplayer.get_unique_id(),GameManager.player_info[multiplayer.get_unique_id()]["name"], 2)
		
func on_player_disconnected(id):
	GameManager.num_players -= 1
	# The other player disconnected
	if(id != 1):
		get_node("PlayerBoxes/P2/Name").text = "-"
		get_node("PlayerBoxes/P2/Ready").text = ""
		get_node("ReadyButton").disabled = false
	# The server player disconnected
	else:
		get_tree().change_scene_to_file("res://start_screen.tscn")
		

func on_connected_to_server():
	pass
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
@rpc("any_peer", "call_local", "reliable")
func update_text(new_text, node_path, id):
	if(GameManager.player_info.has(id)):
		GameManager.player_info[id]["name"] = new_text
	else:
		if(multiplayer.is_server()):
			GameManager.player_info[id] = {
				"name": new_text,
				"player_number": 1
			}
		else:
			GameManager.player_info[id] = {
				"name": new_text,
				"player_number": 2
			}
	get_node(node_path).text = new_text

@rpc("any_peer", "call_local", "reliable")
func update_ready():
	is_ready = !is_ready
	if(is_ready):
		get_node("PlayerBoxes/P2/Ready").text = "✓"
		if(multiplayer.is_server()):
			get_node("ReadyButton").disabled = false
	else:
		get_node("PlayerBoxes/P2/Ready").text = ""
		if(multiplayer.is_server()):
			get_node("ReadyButton").disabled = true

@rpc("any_peer", "call_local", "reliable")
func update_player_info(id, name, player_num):
	
	GameManager.player_info[id] = {
		"name": name,
		"player_number": player_num
	}

@rpc("call_local", "reliable")
func load_game(game_scene_path):
	GameManager.num_players += 1
	GameManager.is_in_game = true
	get_tree().change_scene_to_file(game_scene_path)
	
func after_game_start(p1_name):
	get_node("PlayerBoxes/P1/Name").text = p1_name
	GameManager.player_info[1] = {
		"name": p1_name,
		"player_number": 1
	}
	GameManager.update_player_info.rpc(multiplayer.get_unique_id(), "cube 2", 2)
	GameManager.game_already_started = true
	get_node("ReadyButton").text = "JOIN"


func on_text_submitted(new_text):
	if(multiplayer.is_server()):
		has_updated_text = true
		update_text.rpc(new_text, "PlayerBoxes/P1/Name", multiplayer.get_unique_id())
		GameManager.player_info[multiplayer.get_unique_id()] = {
			"name" : new_text,
			"player_number" : 1
		}
	else:
		# Check if the server is up yet. 
		if(multiplayer.multiplayer_peer.get_connection_status() != multiplayer.multiplayer_peer.CONNECTION_CONNECTED):
			update_text(new_text, "PlayerBoxes/P2/Name", multiplayer.get_unique_id())
		else:
			update_text.rpc(new_text, "PlayerBoxes/P2/Name", multiplayer.get_unique_id())
		GameManager.player_info[multiplayer.get_unique_id()] = {
			"name" : new_text,
			"player_number" : 2
		}
	
func on_button_pressed():
	if(multiplayer.is_server()):
		if(!has_updated_text):
			GameManager.player_info[multiplayer.get_unique_id()] = {
			"name" : "cube 1",
			"player_number" : 1
		}
		GameManager.is_in_game = true
		load_game.rpc("res://main.tscn")
	elif(!multiplayer.is_server()):
		if(GameManager.game_already_started):
			GameManager.num_players += 1
			GameManager.is_in_game = true
			get_tree().change_scene_to_file("res://main.tscn")
		else:
			update_ready.rpc()
