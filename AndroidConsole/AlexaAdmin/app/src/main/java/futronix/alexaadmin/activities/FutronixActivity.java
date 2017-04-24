package futronix.alexaadmin.activities;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ProgressBar;

import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.callbacks.GetStatusCallback;

public abstract class FutronixActivity extends AppCompatActivity
{
    private ProgressBar progressSpinner;

    protected abstract int getLayoutResId();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.setContentView(getLayoutResId());
        /*
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        */

        this.progressSpinner = (ProgressBar)findViewById(R.id.progressSpinner);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    protected void directToDiscover()
    {
        Intent intent = new Intent(this, FindDevicesActivity.class);
        startActivity(intent);
        finish();
    }

    protected void directToSetup()
    {
        Intent intent = new Intent(this, SetupActivity.class);
        startActivity(intent);
        finish();
    }

    protected void directToScenes()
    {
        Intent intent = new Intent(this, ScenesActivity.class);
        startActivity(intent);
        finish();
    }

    protected void directToMain()
    {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        // finish();
    }

    protected void showSpinner()
    {
        try {
            this.progressSpinner.setVisibility(View.VISIBLE);
        }
        catch (Exception e)
        {
            //TODO: log error
        }
    }

    protected void dismissSpinner()
    {
        try {
            this.progressSpinner.setVisibility(View.GONE);
        }
        catch (Exception e) {
            //TODO: log error
        }
    }
}
