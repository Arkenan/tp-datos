from datetime import datetime
import pandas as pd
pd.options.mode.chained_assignment = None
import numpy as np
import pickle
import gzip
import xgboost as xgb

from sklearn.feature_extraction import DictVectorizer
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.cross_validation import train_test_split
from sklearn.metrics import log_loss

print 'starting...', datetime.now()

train = pd.read_csv('../data/train.csv.gz', parse_dates=True)

label_encoder = LabelEncoder()

def get_dates(d):
    dt = datetime.strptime(d, '%Y-%m-%d %H:%M:%S')
    return dt.hour, dt.month, dt.year

def get_features(data):
    df = pd.DataFrame()
    df['Hour'], df['Month'], df['Year'] = zip(*data['Dates'].apply(get_dates))
    df = df.join(pd.get_dummies(data['PdDistrict'], prefix='District'))
    df = df.join(pd.get_dummies(data['DayOfWeek'], prefix='Day'))
    labels = None
    if 'Category' in data:
        labels = label_encoder.fit_transform(data['Category'])
    return df, labels

features, labels = get_features(train)
scaler = StandardScaler()
scaler.fit(features)
features = scaler.transform(features)

print 'training...', datetime.now()

# nthread = 1 es para que no me consuma todos los recursos de la maquina :S
gbm = xgb.XGBClassifier(max_depth=9, nthread=-1, n_estimators=100, learning_rate=0.3, silent=True, subsample=0.9)
gbm.fit(features, labels, eval_metric="mlogloss")

pickle.dump(gbm, open('xbm_model.pkl', 'wb'))

print 'prepairing test data...', datetime.now()

test = pd.read_csv('../data/test.csv.gz', parse_dates=True)
test_features, _ = get_features(test)

scaler.fit(test_features)
test_features = scaler.transform(test_features)

print 'predicting...', datetime.now()

proba = gbm.predict_proba(test_features)
prediction = pd.DataFrame(proba, columns=label_encoder.classes_)

print 'exporting...', datetime.now()

with gzip.GzipFile('xgbv1.csv.gz', mode='w', compresslevel=9) as f:
    prediction.to_csv(f, index_label="Id", na_rep="0")
