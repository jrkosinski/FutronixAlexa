package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Spinner;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.callbacks.ApiStatusCallback;

public class ScenesActivity extends FutronixActivity
{
    private Button setSceneButton;
    private Spinner sceneSpinner;

    @Override
    protected int getLayoutResId() { return R.layout.activity_scenes;}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.setSceneButton = (Button)this.findViewById(R.id.setSceneButton);
        this.sceneSpinner = (Spinner)this.findViewById(R.id.sceneSpinner);

        this.setSceneButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int scene = sceneSpinner.getSelectedItemPosition();

                Global.apiInterface.setSceneAsync(scene, new ApiStatusCallback() {
                    @Override
                    public void execute(ApiStatus status) {
                        dismissSpinner();

                        switch (status) {
                            case NotFound:
                                directToDiscover();
                                break;
                            case NotSetUp:
                                directToSetup();
                                break;
                            case Running:
                                break;
                        }
                    }
                });
            }
        });
    }
}
