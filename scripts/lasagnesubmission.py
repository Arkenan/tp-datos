
# coding: utf-8

# In[1]:

from datetime import datetime
import pandas as pd
pd.options.mode.chained_assignment = None
import numpy as np
import gzip


# In[2]:

from holidays import US
us_holidays = US()


# In[72]:

from lasagne.layers import DenseLayer, InputLayer, DropoutLayer
from lasagne.nonlinearities import softmax
from lasagne.updates import nesterov_momentum
from nolearn.lasagne import NeuralNet, TrainSplit
from theano.tensor import TensorType, vector

from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.cross_validation import train_test_split
from sklearn.metrics import log_loss


# In[112]:

train = pd.read_csv('../data/train.csv.gz', parse_dates=True)


# In[6]:

test = pd.read_csv('../data/test.csv.gz', parse_dates=True)


# In[113]:

street_encoder = LabelEncoder()

def get_street_name(addr):
    sp = addr.split(' ')
    if sp[0].isdigit():
        return ' '.join(sp[1:])
    return addr


# In[114]:

streets = pd.concat([train['Address'].apply(get_street_name), test['Address'].apply(get_street_name)])
street_encoder.fit(streets.unique())


# In[115]:

seasons = dict(zip([1,2,3,4,5,6,7,8,9,10,11,12], ['Winter','Winter','Spring','Spring','Spring','Summer','Summer','Summer','Autumn','Autumn','Autumn','Winter']))
def get_season(d):
    return seasons[d]


# In[116]:

label_encoder = LabelEncoder()

def get_dates(d):
    dt = datetime.strptime(d, '%Y-%m-%d %H:%M:%S')
    season = get_season(dt.month)
    # http://www.timeanddate.com/sun/usa/san-francisco
    if season == 'Winter':
        isDaytime = int(6 < dt.hour < 18)
    elif season == 'Summer':
        isDaytime = int(5 < dt.hour < 21)
    else:
        isDaytime = int(6 < dt.hour < 19)
    is_holiday = dt in us_holidays
    return dt.hour, dt.month, dt.year, season, isDaytime, is_holiday

def get_features(data):
    df = pd.DataFrame()
    df['Hour'], df['Month'], df['Year'], seasons, df['IsDaytime'], df['IsHoliday'] = zip(*data['Dates'].apply(get_dates))
    df['isWeekend'] = data['DayOfWeek'].apply(lambda x: int(x == 'Saturday' or x == 'Sunday'))
    df = df.join(pd.get_dummies(data['PdDistrict'], prefix='District'))
    df = df.join(pd.get_dummies(data['DayOfWeek'], prefix='Day'))
    df = df.join(pd.get_dummies(seasons, prefix='Season'))
    xy_fitted = StandardScaler().fit_transform(data[['X', 'Y']])
    df['X'] = xy_fitted[:, 0]
    df['Y'] = xy_fitted[:, 1]
    df['IsIntersection'] = data['Address'].map(lambda x: int('/' in x))
    df['Street'] = street_encoder.transform(data['Address'].apply(get_street_name))
    labels = None
    if 'Category' in data:
        labels = label_encoder.fit_transform(data['Category'])
    return df, labels


# In[117]:

features, labels = get_features(train)


# In[118]:

scaler = StandardScaler()
scaler.fit(features)
features_scaled = scaler.transform(features)


# In[119]:

X_train, X_test, y_train, y_test = train_test_split(features_scaled, labels, test_size=0.4, random_state=0)


# In[120]:

num_classes = len(label_encoder.classes_)
num_features = X_train.shape[1]


# In[121]:

net0 = NeuralNet(layers=[
                    ('input', InputLayer),
                    ('dense0', DenseLayer),
                    ('dropout', DropoutLayer),
                    ('dense1', DenseLayer),
                    ('output', DenseLayer)],

                 input_shape=(None, num_features),
                 dense0_num_units=200,
                 dropout_p=0.5,
                 dense1_num_units=200,
                 output_num_units=num_classes,
                 output_nonlinearity=softmax,
                 update=nesterov_momentum,
                 update_learning_rate=0.01,
                 update_momentum=0.9,
                 
                 train_split=TrainSplit(eval_size=0.2),
                 verbose=1,
                 max_epochs=20)


# In[122]:

net0


# In[ ]:

print 'started', datetime.now()
res = net0.fit(features_scaled, labels.astype('int32'))
print 'finished', datetime.now()
print res


# In[2]:

import pickle
pickle.dump(net0, open('lasagne_model.pkl', 'wb'))


# In[106]:

print 'score', net0.score(features_scaled, labels)


# In[111]:

print 'logloss', log_loss(labels, net0.predict_proba(features_scaled))


# In[16]:

test_features, _ = get_features(test)

scaler.fit(test_features)
test_features = scaler.transform(test_features)


# In[18]:

proba = net0.predict_proba(test_features)
prediction = pd.DataFrame(proba, columns=label_encoder.classes_)


# In[ ]:

with gzip.GzipFile('lasv1.csv.gz', mode='w', compresslevel=9) as f:
    prediction.to_csv(f, index_label="Id", na_rep="0", float_format='%11.6f')

