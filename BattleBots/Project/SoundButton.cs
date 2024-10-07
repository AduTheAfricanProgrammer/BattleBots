using Godot;
using System;

public partial class SoundButton : Button
{
	bool isSoundOn = true;
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		Text = "Sound ON";
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	public void OnPressed(){
		if(isSoundOn){
			Text = "Sound OFF";
			isSoundOn = false;
		}
		else{
			Text = "Sound ON";
			isSoundOn = true;
		}
	}
}
