using Godot;
using System;

public partial class TestScript : CharacterBody3D
{

	float time = 0;
    public override void _Ready()
    {
       
    }

    public override void _PhysicsProcess(double delta)
	{
		time += (float) delta;
		if(time > 0.1){
			GetNode<NavigationAgent3D>("Nav").TargetPosition = GetParent().GetNode<Node3D>("Player").GlobalPosition;
			Vector3 currentLocation = GlobalPosition;
			Vector3 nextLocation = GetNode<NavigationAgent3D>("Nav").GetNextPathPosition();
			Vector3 newVelocity = (nextLocation - currentLocation).Normalized() * 3;

			Velocity = Velocity.MoveToward(newVelocity, 0.1f);
			MoveAndSlide();
		}
		
	}
}
