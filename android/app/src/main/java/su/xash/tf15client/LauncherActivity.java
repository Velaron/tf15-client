package su.xash.tf15client;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
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

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class LauncherActivity extends AppCompatActivity {
	private RequestQueue mRequestQueue;

	private static final String COMMITS_URL = "https://api.github.com/repos/Velaron/tf15-client/commits/master";
	private static final String RUNS_URL = "https://api.github.com/repos/Velaron/tf15-client/actions/runs?branch=master";
	private static final String APK_URL = "https://github.com/Velaron/tf15-client/releases/download/continuous/tf15-client.apk";

	private static final String ENGINE_COMMITS_URL = "https://api.github.com/repos/FWGS/xash3d-fwgs/commits/master";
	private static final String ENGINE_RUNS_URL = "https://api.github.com/repos/FWGS/xash3d-fwgs/actions/runs?branch=master";

	private interface VolleyCallback {
		void onFinished(boolean result);
	}

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
			mRequestQueue = Volley.newRequestQueue(this);
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
		CoordinatorLayout contextView = findViewById(R.id.coordinatorLayout);
		ExtendedFloatingActionButton launchButton = findViewById(R.id.launchButton);
		launchButton.setEnabled(false);

		if (checkEngineABI()) {
			Snackbar updateNotification = Snackbar.make(contextView, R.string.checking_for_updates, Snackbar.LENGTH_INDEFINITE).setAnchorView(launchButton);
			updateNotification.show();

			checkForEngineUpdates(result -> {
				if (result) {
					checkForUpdates(result1 -> {
						updateNotification.dismiss();
						if (result1)
							launchButton.setEnabled(true);
					});
				} else {
					updateNotification.dismiss();
				}
			});
		}
	}

	private void showUpdateDialog(String title, String message, Uri url) {
		new MaterialAlertDialogBuilder(LauncherActivity.this)
				.setTitle(title)
				.setMessage(message)
				.setCancelable(true)
				.setNegativeButton(R.string.exit, (dialog, which) -> finish())
				.setPositiveButton(R.string.install, (dialog, which) -> startActivity(new Intent(Intent.ACTION_VIEW).setData(url))).show();
	}


	private boolean checkEngineABI() {
		try {
			PackageInfo info = getPackageManager().getPackageInfo("su.xash.engine", 0);

			if (!info.applicationInfo.nativeLibraryDir.contains("64")
					&& Build.SUPPORTED_ABIS[0].contains("64")) {
				showUpdateDialog(
						getString(R.string.engine_wrong_abi),
						getString(R.string.engine_abi),
						Uri.parse(getEngineDownloadUrl())
				);

				return false;
			}
		} catch (PackageManager.NameNotFoundException e) {
			showUpdateDialog(
					getString(R.string.engine_not_found),
					getString(R.string.engine_info),
					Uri.parse(getEngineDownloadUrl())
			);

			return false;
		}

		return true;
	}

	private void checkForEngineUpdates(final VolleyCallback callback) {
		JsonObjectRequest commitsRequest = new JsonObjectRequest(Request.Method.GET, ENGINE_COMMITS_URL, null, commitsResponse -> {
			try {
				String sha = commitsResponse.getString("sha").substring(0, 7);

				JsonObjectRequest runsRequest = new JsonObjectRequest(Request.Method.GET, ENGINE_RUNS_URL, null, runsResponse -> {
					try {
						JSONArray workflowRuns = runsResponse.getJSONArray("workflow_runs");
						String status = workflowRuns.getJSONObject(0).getString("status");
						String conclusion = workflowRuns.getJSONObject(0).getString("conclusion");

						Resources res = getPackageManager().getResourcesForApplication("su.xash.engine");
						int resId = res.getIdentifier("git_revisions", "string", "su.xash.engine");
						String gitRevisions = res.getString(resId);

						// developer build
						if (!gitRevisions.equals("No git version information")) {
							Pattern p = Pattern.compile("(?<=\\bxash3d-fwgs:\\s)(\\w+)");
							Matcher m = p.matcher(gitRevisions);

							if (m.find()) {
								String engineCommitHash = m.group();

								if (!engineCommitHash.equals(sha) && status.equals("completed") && conclusion.equals("success")) {
									showUpdateDialog(
											getString(R.string.update_required),
											getString(R.string.update_available, "Xash3D FWGS"),
											Uri.parse(getEngineDownloadUrl())
									);

									callback.onFinished(false);
								}
							}
						}

						callback.onFinished(true);
					} catch (JSONException | PackageManager.NameNotFoundException e) {
						e.printStackTrace();
						callback.onFinished(true);
					}
				}, error -> callback.onFinished(true));

				mRequestQueue.add(runsRequest);
			} catch (JSONException e) {
				e.printStackTrace();
				callback.onFinished(true);
			}
		}, error -> callback.onFinished(true));

		mRequestQueue.add(commitsRequest);
	}

	private void checkForUpdates(final VolleyCallback callback) {
		JsonObjectRequest commitsRequest = new JsonObjectRequest(Request.Method.GET, COMMITS_URL, null, commitsResponse -> {
			try {
				String sha = commitsResponse.getString("sha").substring(0, 7);

				JsonObjectRequest runsRequest = new JsonObjectRequest(Request.Method.GET, RUNS_URL, null, runsResponse -> {
					try {
						JSONArray workflowRuns = runsResponse.getJSONArray("workflow_runs");
						String status = workflowRuns.getJSONObject(0).getString("status");
						String conclusion = workflowRuns.getJSONObject(0).getString("conclusion");

						if (!BuildConfig.COMMIT_SHA.equals(sha) && status.equals("completed") && conclusion.equals("success")) {
							showUpdateDialog(
									getString(R.string.update_required),
									getString(R.string.update_available, getString(R.string.app_name)),
									Uri.parse(APK_URL)
							);

							callback.onFinished(false);
						}

						callback.onFinished(true);
					} catch (JSONException e) {
						e.printStackTrace();
						callback.onFinished(true);
					}
				}, error -> callback.onFinished(true));

				mRequestQueue.add(runsRequest);
			} catch (JSONException e) {
				e.printStackTrace();
				callback.onFinished(true);
			}
		}, error -> callback.onFinished(true));

		mRequestQueue.add(commitsRequest);
	}
}