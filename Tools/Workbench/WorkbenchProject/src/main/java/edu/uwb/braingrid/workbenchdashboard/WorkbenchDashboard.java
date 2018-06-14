package edu.uwb.braingrid.workbenchdashboard;

import javafx.scene.Scene;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.stage.Stage;
import java.nio.file.Paths;
import java.nio.file.Path;

public class WorkbenchDashboard extends Application {
	/**
	 * GSLE Growth Simulation Layout Editor
	 */
	private WorkbenchDisplay workbench_display_;

	public static void main(String[] args) {
		launch(args);
	}
	
	boolean ctrl = false;
	
	@Override
	public void start(Stage primaryStage) throws Exception {

				Path currentRelativePath = Paths.get("");
		String s = currentRelativePath.toAbsolutePath().toString();
		System.out.println("Current relative path is: " + s);

		workbench_display_ = new WorkbenchDisplay(primaryStage);
		Scene scene = new Scene(workbench_display_, 900, 600);
		
		scene.getStylesheets().add("/simstarter/css/temp.css");
		scene.getStylesheets().add("/simstarter/css/tempII.css");
		
		scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
			@Override
			public void handle(KeyEvent arg0) {
				if(arg0.getCode() == KeyCode.CONTROL) {
					ctrl = true;
				}
				if(arg0.getCode() == KeyCode.G && ctrl ) {
					workbench_display_.pushGSLEPane();
				}
				if(arg0.getCode() == KeyCode.W && ctrl) {
					workbench_display_.pushWeclomePage();
				}
				if(arg0.getCode() == KeyCode.S && ctrl) {
					workbench_display_.pushSimStarterPage();
				}
				if(arg0.getCode() == KeyCode.P && ctrl) {
					workbench_display_.pushProVisStarterPage();
				}
			}
		});
		
		scene.setOnKeyReleased(new EventHandler<KeyEvent>() {
			@Override
			public void handle(KeyEvent arg0) {
				if(arg0.getCode() == KeyCode.CONTROL) {
					ctrl = false;
				}
			}
			
		});
		
		primaryStage.setScene(scene);
		primaryStage.setMaximized(true);
		
		primaryStage.show();

	}
}