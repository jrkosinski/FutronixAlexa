package futronix.alexaadmin.activities;

import android.os.Bundle;
import android.transition.Scene;
import android.view.View;
import android.widget.Button;
import android.widget.Spinner;

import java.util.Hashtable;

import futronix.alexaadmin.Global;
import futronix.alexaadmin.R;
import futronix.alexaadmin.api.ApiStatus;
import futronix.alexaadmin.callbacks.ApiStatusCallback;
import futronix.alexaadmin.presenters.ScenesPresenter;
import futronix.alexaadmin.presenters.ScenesPresenterListener;
import futronix.alexaadmin.presenters.Screen;

public class ScenesActivity extends FutronixActivity
{
    private final ScenesPresenter presenter = new ScenesPresenter();

    private Button setSceneButton;
    private Spinner sceneSpinner;

    @Override
    protected int getLayoutResId() { return R.layout.activity_scenes;}

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        this.presenter.setListener(new PresenterListener());

        this.setSceneButton = (Button)this.findViewById(R.id.setSceneButton);
        this.sceneSpinner = (Spinner)this.findViewById(R.id.sceneSpinner);

        this.setSceneButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                presenter.setScene();
            }
        });
    }

    @Override
    protected void onStart()
    {
        super.onStart();

        this.presenter.start();
    }


    private class PresenterListener extends ScenesPresenterListener
    {
        @Override
        public void requestViewState(Hashtable<String, Object> values) {
            values.put("context", getApplicationContext());
            values.put("scene", sceneSpinner.getSelectedItemPosition());
        }

        @Override
        public void showErrorMessage(String message) {
        }

        @Override
        public void redirect(Screen screen) {
            navigateTo(screen);
        }

        @Override
        public void onSetSceneSuccess(int scene) {
        }

        @Override
        public void onSetSceneFailure(int scene) {
        }

        @Override
        public void showScene(int scene) {
            //TODO: show scene in spinner
        }

        @Override
        public void showProgress(boolean show)
        {
            if (show == true)
                showSpinner();
            else
                dismissSpinner();
        }
    }
}
