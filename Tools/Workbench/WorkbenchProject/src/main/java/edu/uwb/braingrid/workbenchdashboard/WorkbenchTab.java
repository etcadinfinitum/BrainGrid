package edu.uwb.braingrid.workbenchdashboard;

import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.RadioButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class WorkbenchTab extends HBox {
	public WorkbenchTab(String display,  WorkbenchApp pane, WorkbenchDisplay bp) {
		super();
		
		display_name_ = display;
		workbench_app_ = pane;
		workbench_display_ = bp;
		
		// Initialize Radio Button
		selector_ = new RadioButton(display);
		selector_.setOnAction(event -> {
			workbench_display_.setCenter(pane.getDisplay());
		});
		selector_.setAlignment(Pos.CENTER_LEFT);
		
		// Initialize Exit Button
		exit_ = new Button("Close");
		exit_.setOnAction(event -> {
			if(workbench_app_.close()) {
				tabs_.remove(this);
				if(selector_.isSelected()) {
					setToggled(0);
				}
				refreshTabBar();
			}
		});
		exit_.setAlignment(Pos.CENTER_RIGHT);
		
		this.setPadding(new Insets(3));
		this.setSpacing(10.0);
		this.getChildren().add(selector_);
		this.getChildren().add(exit_);

		tabs_.add(this);
		refreshTabBar();
		selector_.setSelected(true);
		workbench_display_.setCenter(this.getPane().getDisplay());
	}
	
	public void setTabHandeler(ToggleGroup tab_buttons) {
		selector_.setToggleGroup(tab_buttons);
	}
	
	public static void setToggled(int index) {
		if(index >= 0 && index < tabs_.size()) {
			tabs_.get(index).getSelector().setSelected(true);
			workbench_display_.setCenter(tabs_.get(index).getPane().getDisplay());
		}
	}
	
	public WorkbenchApp getPane() {
		return workbench_app_;
	}
	
	public RadioButton getSelector() {
		return selector_;
	}
	
	public static void refreshTabBar() {
		HBox tab_markers_ = new HBox();
		tab_markers_.setStyle("-fx-background-color: red");
		tab_markers_.setStyle("-fx-padding: 3 0 0 0;-fx-background-color: red");

		ListIterator<WorkbenchTab> it = tabs_.listIterator();
		ToggleGroup tab_buttons = new ToggleGroup();
		while(it.hasNext()) {
			WorkbenchTab temp = it.next();
			tab_markers_.getChildren().add( temp );
			temp.setTabHandeler(tab_buttons);
		}
		VBox top = new VBox(workbench_display_.getMenuBar(), tab_markers_);
		workbench_display_.setTop(top);
	}
	
	private static WorkbenchDisplay workbench_display_;
	private static List<WorkbenchTab> tabs_ = new LinkedList<WorkbenchTab>();
	
	private String display_name_;
	private WorkbenchApp workbench_app_;
	private RadioButton selector_;
	private Button exit_;
	
}
