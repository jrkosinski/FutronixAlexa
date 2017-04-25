package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;

public class FindDevicesActivity extends FutronixActivity
{
    private Button findButton;

    @Override
    protected int getLayoutResId() { return R.layout.activity_find_devices;}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.findButton = (Button)findViewById(R.id.findButton);

        this.findButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                Global.device.clear();
                directToMain();
            }
        });
    }
}
