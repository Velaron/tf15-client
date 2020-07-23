package su.xash.tf15client;
import android.app.*;
import android.content.*;
import android.content.pm.*;
import android.graphics.*;
import android.os.*;
import android.text.*;
import android.text.style.*;
import android.view.*;
import android.widget.*;
import android.widget.LinearLayout.*;

public class LauncherActivity extends Activity
{	
    @Override
    protected void onCreate( Bundle savedInstanceState )
	{
        super.onCreate( savedInstanceState );
		getWindow().requestFeature( Window.FEATURE_NO_TITLE );
		setContentView( R.layout.activity_launcher );

		Spinner spinner = (Spinner)findViewById( R.id.dev_spinner );
		ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource( this, R.array.dev_options, R.layout.spinner_item );
		adapter.setDropDownViewResource( android.R.layout.simple_spinner_dropdown_item );
		spinner.setAdapter( adapter );

		ExtractAssets.extractPAK( this, false );
	}
/*
	private Intent prepareIntent( Intent i )
	{
		String argv = cmdArgs.getText().toString();
		i.addFlags( Intent.FLAG_ACTIVITY_NEW_TASK );

		SharedPreferences.Editor editor = mPref.edit();
		editor.putString( "argv", argv );
		editor.commit();

		if( argv.length() != 0 )
			i.putExtra( "argv", argv );

		i.putExtra( "gamedir", "tfc" );

		try
		{
			PackageManager packageManager = getPackageManager();
			ApplicationInfo applicationInfo = packageManager.getApplicationInfo(getPackageName(), 0);
			i.putExtra( "gamelibdir", applicationInfo.nativeLibraryDir );
		}
		catch(Exception e)
		{
			e.printStackTrace();
			i.putExtra( "gamelibdir", getFilesDir().getParentFile().getPath() + "/lib" );
		}
		
		i.putExtra("pakfile", getExternalFilesDir(null).getAbsolutePath() + "/extras.pak");

		return i;
	}
*/
    public void startXash( View view )
    {
		Intent intent = new Intent();
		intent.setComponent( new ComponentName( "su.xash.engine", "su.xash.engine.XashActivity" ) ); 
		intent.putExtra( "pakfile", getExternalFilesDir( null ).getAbsolutePath() + "/extras.pak" );
		intent.putExtra( "gamedir", "tfc" );
		intent.putExtra( "argv", "-log -dev 5" );

		try
		{
			PackageManager packageManager = getPackageManager();
			ApplicationInfo applicationInfo = packageManager.getApplicationInfo( getPackageName(), 0 );
			intent.putExtra( "gamelibdir", applicationInfo.nativeLibraryDir );
		}
		catch( Exception e )
		{
			intent.putExtra( "gamelibdir", getFilesDir().getParentFile().getPath() + "/lib" );
		}

		startActivity( intent );
	}
}