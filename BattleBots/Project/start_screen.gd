extends Control

@export var temp_address = "127.0.0.1"

# Called when the node enters the scene tree for the first time.
func _ready():
	multiplayer.peer_connected.connect(on_player_connected)
	multiplayer.connected_to_server.connect(on_connected_to_server)
	multiplayer.connection_failed.connect(on_connection_failed)

func on_player_connected(id):
	print("Player connected! " + str(id))

func on_player_disconnected(id):
	print("Player disconnected! " + str(id))

func on_connected_to_server():
	print("Connected to server!")
	
func on_connection_failed():
	print("Failed to connect!")
	
func on_singleplayer_pressed():
	GameManager.is_in_singleplayer = true
	get_tree().change_scene_to_file("res://main.tscn")
	
func on_create_pressed():
	var peer = ENetMultiplayerPeer.new()
	# If we get an error here, it just means that the server has already been made
	var error = peer.create_server(8192, 2)
	if(error != OK):
		get_node("WarningLabel").text = "Lobby already in place. Join the game instead."
		return
	multiplayer.set_multiplayer_peer(peer)
	get_tree().change_scene_to_file("res://lobby.tscn")
	print("Waiting for players!")
	
func on_join_pressed():
	var peer = ENetMultiplayerPeer.new()
	var error = peer.create_client(temp_address, 8192)
	if(error != OK):
		return
	multiplayer.set_multiplayer_peer(peer)
	get_tree().change_scene_to_file("res://lobby.tscn")
