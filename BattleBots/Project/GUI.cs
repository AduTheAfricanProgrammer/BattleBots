using Godot;
using System;

public partial class GUI : Control
{
	// Get Dashes label node
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
	

	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	public void OnHealthUpdate(int currentHealth){
		GetNode<ProgressBar>("HealthBar").Value = currentHealth;
	}

	public void OnFlightUpdate(float flightTime){
		GetNode<ProgressBar>("FlightBar").Value = flightTime;
	}

	public void OnDashUpdate(float currentDashes){
		GetNode<ProgressBar>("DashBar").Value = currentDashes;
	}

}
