package su.xash.tf15client;

import androidx.appcompat.app.AppCompatActivity;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.Manifest;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.textfield.TextInputEditText;

public class LauncherActivity extends AppCompatActivity {
	@Override
	public void onCreate(Bundle savedInstanceBundle) {
		super.onCreate(savedInstanceBundle);
		setContentView(R.layout.activity_launcher);

		ExtractAssets.extractPAK(this, false);
		PermissionManager.checkPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);

		TextInputEditText launchParameters = (TextInputEditText)findViewById(R.id.launchParameters);
		launchParameters.setText("-log -dev 2");
		
		ExtendedFloatingActionButton launchButton = (ExtendedFloatingActionButton)findViewById(R.id.launchButton);
		launchButton.setOnClickListener((view) -> {
			startActivity(new Intent().setComponent(new ComponentName("su.xash.engine", "su.xash.engine.XashActivity"))
			.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
			.putExtra("pakfile", getExternalFilesDir( null ).getAbsolutePath() + "/extras.pak")
			.putExtra("gamedir", "tfc")
			.putExtra("argv", launchParameters.getText())
			.putExtra("gamelibdir", getApplicationInfo().nativeLibraryDir));
		});
	}
}