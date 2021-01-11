package su.xash.tf15client;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts.RequestPermission;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.Manifest;

class PermissionManager {
	public static void checkPermission(AppCompatActivity context, String permission) {
		if (ContextCompat.checkSelfPermission(context, permission) == PackageManager.PERMISSION_GRANTED) {
			
		} else if (context.shouldShowRequestPermissionRationale(permission)) {
			
		} else {
			ActivityResultLauncher<String> requestPermissionLauncher = context.registerForActivityResult(new RequestPermission(), isGranted -> {
				if (isGranted) {

				} else {
					
				}
			});

			requestPermissionLauncher.launch(permission);
		}
	}
}