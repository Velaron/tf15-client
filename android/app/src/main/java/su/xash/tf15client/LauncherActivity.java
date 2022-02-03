package su.xash.tf15client;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.coordinatorlayout.widget.CoordinatorLayout;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.floatingactionbutton.ExtendedFloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.textfield.TextInputEditText;

import org.json.JSONArray;
import org.json.JSONException;

public class LauncherActivity extends AppCompatActivity {
	private static final int XASH_MIN_VERSION = 1710;
	private static final String COMMITS_URL = "https://api.github.com/repos/Velaron/tf15-client/commits/master";
	private static final String RUNS_URL = "https://api.github.com/repos/Velaron/tf15-client/actions/runs";
	private static final String APK_URL = "https://github.com/Velaron/tf15-client/releases/download/continuous/tf15-client.apk";

	@SuppressLint("SetTextI18n")
	@Override
	public void onCreate(Bundle savedInstanceBundle) {
		super.onCreate(savedInstanceBundle);
		setContentView(R.layout.activity_launcher);

		ExtractAssets.extractPAK(this, false);

		TextInputEditText launchParameters = findViewById(R.id.launchParameters);
		launchParameters.setText("-log -dev 2");

		ExtendedFloatingActionButton launchButton = findViewById(R.id.launchButton);
		launchButton.setOnClickListener((view) -> startActivity(new Intent().setComponent(new ComponentName("su.xash.engine", "su.xash.engine.XashActivity"))
				.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
				.putExtra("pakfile", getExternalFilesDir(null).getAbsolutePath() + "/extras.pak")
				.putExtra("gamedir", "tfc")
				.putExtra("argv", launchParameters.getText())
				.putExtra("gamelibdir", getApplicationInfo().nativeLibraryDir)));

		ExtendedFloatingActionButton donateButton = findViewById(R.id.donateButton);
		donateButton.setOnClickListener(v -> startActivity(new Intent(Intent.ACTION_VIEW)
				.setData(Uri.parse("https://www.buymeacoffee.com/velaron"))));

		if (!BuildConfig.DEBUG) {
			checkForEngine();
		}
	}

	private String getEngineDownloadUrl() {
		if (!Build.SUPPORTED_ABIS[0].contains("64")) {
			return "https://github.com/FWGS/xash3d-fwgs/releases/download/continuous/xashdroid-32.apk";
		} else {
			return "https://github.com/FWGS/xash3d-fwgs/releases/download/continuous/xashdroid-64.apk";
		}
	}

	private void checkForEngine() {
		try {
			PackageInfo info = getPackageManager().getPackageInfo("su.xash.engine", 0);

			if (info.versionCode < XASH_MIN_VERSION) {
				new MaterialAlertDialogBuilder(LauncherActivity.this)
						.setTitle(R.string.update_required)
						.setMessage(getString(R.string.update_available, "Xash3D FWGS"))
						.setCancelable(true)
						.setNegativeButton(R.string.later, null)
						.setPositiveButton(R.string.update, (dialog, which) -> startActivity(new Intent(Intent.ACTION_VIEW).setData(Uri.parse(getEngineDownloadUrl())))).show();
			} else {
				checkForUpdates();
			}
		} catch (PackageManager.NameNotFoundException e) {
			new MaterialAlertDialogBuilder(LauncherActivity.this)
					.setTitle(R.string.engine_not_found)
					.setMessage(R.string.engine_info)
					.setCancelable(true)
					.setNegativeButton(R.string.later, null)
					.setPositiveButton(R.string.update, (dialog, which) -> startActivity(new Intent(Intent.ACTION_VIEW).setData(Uri.parse(getEngineDownloadUrl())))).show();
		}
	}

	private void checkForUpdates() {
		CoordinatorLayout contextView = findViewById(R.id.coordinatorLayout);
		ExtendedFloatingActionButton launchButton = findViewById(R.id.launchButton);

		Snackbar updateNotification = Snackbar.make(contextView, R.string.checking_for_updates, Snackbar.LENGTH_INDEFINITE).setAnchorView(launchButton);
		updateNotification.show();

		RequestQueue requestQueue = Volley.newRequestQueue(getApplicationContext());

		JsonObjectRequest commitsRequest = new JsonObjectRequest(Request.Method.GET, COMMITS_URL, null, commitsResponse -> {
			try {
				String sha = commitsResponse.getString("sha").substring(0, 7);

				JsonObjectRequest runsRequest = new JsonObjectRequest(Request.Method.GET, RUNS_URL, null, runsResponse -> {
					try {
						JSONArray workflowRuns = runsResponse.getJSONArray("workflow_runs");
						String status = workflowRuns.getJSONObject(0).getString("status");
						String conclusion = workflowRuns.getJSONObject(0).getString("conclusion");

						if (!BuildConfig.COMMIT_SHA.equals(sha) && status.equals("completed") && conclusion.equals("success")) {
							new MaterialAlertDialogBuilder(LauncherActivity.this)
									.setTitle(R.string.update_required)
									.setMessage(getString(R.string.update_available, getString(R.string.app_name)))
									.setCancelable(true)
									.setNegativeButton(R.string.later, null)
									.setPositiveButton(R.string.update, (dialog, which) -> startActivity(new Intent(Intent.ACTION_VIEW).setData(Uri.parse(APK_URL)))).show();
						}

						updateNotification.dismiss();
					} catch (JSONException e) {
						e.printStackTrace();
					}
				}, error -> updateNotification.dismiss());

				requestQueue.add(runsRequest);

			} catch (JSONException e) {
				e.printStackTrace();
			}
		}, error -> updateNotification.dismiss());

		requestQueue.add(commitsRequest);
	}
}