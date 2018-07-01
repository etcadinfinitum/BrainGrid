package edu.uwb.braingrid.workbenchdashboard.welcome;

import edu.uwb.braingrid.workbenchdashboard.WorkbenchApp;
import edu.uwb.braingrid.workbenchdashboard.nledit.NLedit;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Tab;
import javafx.scene.layout.HBox;

public class Welcome extends WorkbenchApp {
	public static final String HEADER = "WELCOME --\t";
	private HBox display_ = new HBox();
	
	public Welcome(Tab tab) {
		super(tab);
		System.out.println(Welcome.HEADER + "new Welcome tab opened");
		Label proof = new Label("Welcome!");
		proof.setAlignment(Pos.CENTER);
		display_.getChildren().add(proof);
		display_.setAlignment(Pos.CENTER);
		super.setTitle("Welcome!");
	}

	@Override
	public boolean close() {
		return true;
	}

	@Override
	public Node getDisplay() {
		// TODO Auto-generated method stub
		return display_;
	}

}
