package edu.uwb.braingrid.workbenchdashboard;

import javafx.scene.Node;
import javafx.scene.layout.Pane;

public abstract class WorkbenchApp {
	public abstract boolean close();
	public abstract Node getDisplay();
}
