import pandas as pd
from sklearn.ensemble import RandomForestClassifier

def load_model_and_features():
    df = pd.read_csv('../heart.csv').dropna()
    df = pd.get_dummies(df, drop_first=True)
    X = df.drop('target', axis=1)
    y = df['target']
    rf = RandomForestClassifier(n_estimators=100, random_state=42)
    rf.fit(X, y)
    return rf, X.columns.tolist()

rf_model, feature_columns = load_model_and_features()

def predict_heart_disease_rf(user_data):
    user_df = pd.DataFrame([user_data])
    user_df = pd.get_dummies(user_df, drop_first=True)
    user_df = user_df.reindex(columns=feature_columns, fill_value=0)
    prediction = rf_model.predict(user_df)[0]
    risk_score = rf_model.predict_proba(user_df)[0][1]
    label = "Heart Disease" if prediction == 1 else "No Heart Disease"
    return label, risk_score
