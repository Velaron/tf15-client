package su.xash.tf15client;

import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.coordinatorlayout.widget.CoordinatorLayout;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.textfield.TextInputEditText;

import org.json.JSONException;
import org.json.JSONObject;

public class LauncherActivity extends AppCompatActivity {
	private static final int XASH_MIN_VERSION = 1710;

	@Override
	public void onCreate(Bundle savedInstanceBundle) {
		super.onCreate(savedInstanceBundle);
		setContentView(R.layout.activity_launcher);

		ExtractAssets.extractPAK(this, false);

		TextInputEditText launchParameters = (TextInputEditText) findViewById(R.id.launchParameters);
		launchParameters.setText("-log -dev 2");

		ExtendedFloatingActionButton launchButton = (ExtendedFloatingActionButton) findViewById(R.id.launchButton);
		launchButton.setOnClickListener((view) -> {
			startActivity(new Intent().setComponent(new ComponentName("su.xash.engine", "su.xash.engine.XashActivity"))
					.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
					.putExtra("pakfile", getExternalFilesDir(null).getAbsolutePath() + "/extras.pak")
					.putExtra("gamedir", "tfc")
					.putExtra("argv", launchParameters.getText())
					.putExtra("gamelibdir", getApplicationInfo().nativeLibraryDir));
		});

		launchButton.setEnabled(false);

		checkForEngine();
	}

	private void checkForEngine() {
		try {
			PackageInfo info = getPackageManager().getPackageInfo("su.xash.engine", 0);

			if (info.versionCode < XASH_MIN_VERSION) {
				openDialog("https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous", getString(R.string.update_required),
						getString(R.string.update_available, "Xash3D FWGS"));
			} else {
				checkForUpdates();
			}
		} catch (PackageManager.NameNotFoundException e) {
			openDialog("https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous",
					getString(R.string.engine_not_found), getString(R.string.engine_info));
		}
	}

	private void checkForUpdates() {
		String url = "https://api.github.com/repos/Velaron/tf15-client/commits/master";
		CoordinatorLayout contextView = (CoordinatorLayout) findViewById(R.id.coordinatorLayout);
		ExtendedFloatingActionButton launchButton = (ExtendedFloatingActionButton) findViewById(R.id.launchButton);

		Snackbar updateNotification = Snackbar.make(contextView, R.string.checking_for_updates, Snackbar.LENGTH_INDEFINITE).setAnchorView(launchButton);
		updateNotification.show();

		JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET, url, null, new Response.Listener<JSONObject>() {
			@Override
			public void onResponse(JSONObject response) {
				try {
					String sha = response.getString("sha").substring(0, 7);
					String version_sha = getPackageManager().getPackageInfo(getPackageName(), 0).versionName.split("-")[1];

					if (version_sha.equals(sha)) {
						updateNotification.dismiss();
						launchButton.setEnabled(true);
					} else {
						updateNotification.dismiss();
						openDialog("https://github.com/Velaron/tf15-client/releases/tag/continuous", getString(R.string.update_required),
								getString(R.string.update_available, "TF15Client"));
					}
				} catch (JSONException e) {
					e.printStackTrace();
				} catch (PackageManager.NameNotFoundException e) {
					e.printStackTrace();
				}
			}
		}, new Response.ErrorListener() {
			@Override
			public void onErrorResponse(VolleyError error) {
				updateNotification.dismiss();
				launchButton.setEnabled(true);
			}
		});

		RequestQueue requestQueue = Volley.newRequestQueue(getApplicationContext());
		requestQueue.add(jsonObjectRequest);
	}

	private void openDialog(String url, String title, String description) {
		new MaterialAlertDialogBuilder(LauncherActivity.this)
				.setTitle(title)
				.setMessage(description)
				.setCancelable(false)
				.setNegativeButton(R.string.exit, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						finish();
					}
				})
				.setPositiveButton(R.string.update, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						startActivity(new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url)));
					}
				}).show();
	}
}