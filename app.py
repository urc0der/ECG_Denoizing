from flask import Flask, render_template, request
from model.predictor import predict_heart_disease_rf
import os

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    risk = None
    label = None
    form_data = {}
    show_prediction = False
    if request.method == 'POST':
        if 'reset' in request.form:
            form_data = {}
            show_prediction = False
        else:
            form_data = request.form.to_dict()
            user_data = {
                'age': int(request.form['age']),
                'sex': int(request.form['sex']),
                'cp': int(request.form['cp']),
                'trestbps': int(request.form['trestbps']),
                'chol': int(request.form['chol']),
                'fbs': int(request.form['fbs']),
                'restecg': int(request.form['restecg']),
                'thalach': int(request.form['thalach']),
                'exang': int(request.form['exang']),
                'oldpeak': float(request.form['oldpeak']),
                'slope': int(request.form['slope']),
                'ca': int(request.form['ca']),
                'thal': int(request.form['thal'])
            }
            label, risk = predict_heart_disease_rf(user_data)
            show_prediction = True
    return render_template('index.html', risk=risk, label=label, form_data=form_data, show_prediction=show_prediction)

if __name__ == "__main__":
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port, debug=True)
