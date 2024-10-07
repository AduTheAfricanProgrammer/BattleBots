extends Control


# Called when the node enters the scene tree for the first time.
func _ready():
	if(!GameManager.is_in_singleplayer):
		multiplayer.multiplayer_peer.close()
	if(GameManager.end_state == 1):
		get_node("OverMessage").text = "You won the game!"
		get_node("WinPlayer").play()
	elif(GameManager.end_state == 2):
		get_node("OverMessage").text = "You lost. Better luck next time!"
		get_node("LossPlayer").play()
	elif(GameManager.end_state == 3):
		get_node("OverMessage").text = "You defeated all the enemies!"
		get_node("WinPlayer").play()
	else:
		get_node("WinPlayer").play()

func _on_menu_pressed():
	get_tree().change_scene_to_file("res://start_screen.tscn")
	GameManager.reset()
	
func _on_exit_pressed():
	get_tree().quit()
