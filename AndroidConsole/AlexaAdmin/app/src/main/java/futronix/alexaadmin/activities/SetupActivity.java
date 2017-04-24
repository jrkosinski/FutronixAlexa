package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.callbacks.ApiStatusCallback;

public class SetupActivity extends FutronixActivity
{
    @Override
    protected int getLayoutResId() { return R.layout.activity_setup;}

    private Button setupButton;
    private EditText wifiSsidText;
    private EditText wifiPasswdText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.setupButton = (Button)findViewById(R.id.setupButton);
        this.wifiSsidText = (EditText)findViewById(R.id.wifiSsidText);
        this.wifiPasswdText = (EditText)findViewById(R.id.wifiPasswdText);

        this.setupButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                Global.apiInterface.setupAsync(
                        wifiSsidText.getText().toString(),
                        wifiPasswdText.getText().toString(),
                        new ApiStatusCallback() {
                            @Override
                            public void execute(ApiStatus status) {
                                switch (status) {
                                    case NotFound:
                                        directToDiscover();
                                        break;
                                    case NotSetUp:
                                        directToSetup();
                                        break;
                                    case Running:
                                        directToScenes();
                                        break;
                                }
                            }
                        });
            }
        });
    }
}
