
# coding: utf-8

# In[102]:

from datetime import datetime
import pandas as pd
pd.options.mode.chained_assignment = None
import numpy as np
import gzip
import time
import copy


# In[3]:

from holidays import US
us_holidays = US()


# In[4]:

from nolearn.metrics import multiclass_logloss


# In[148]:

import theano
from lasagne.layers import DenseLayer, InputLayer, DropoutLayer, GaussianNoiseLayer, Conv2DLayer
from lasagne.nonlinearities import softmax
from lasagne.updates import nesterov_momentum
from lasagne.objectives import categorical_crossentropy
from nolearn.lasagne import NeuralNet, TrainSplit
from theano.tensor import TensorType, vector

from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.cross_validation import train_test_split
from sklearn.metrics import log_loss


# In[115]:

print "Getting training sets"


# In[157]:

train = pd.read_csv('../data/train.csv.gz', parse_dates=True)
# , nrows=200000

# In[158]:

test = pd.read_csv('../data/test.csv.gz', parse_dates=True)


# In[159]:

street_encoder = LabelEncoder()

def get_street_name(addr):
    sp = addr.split(' ')
    if sp[0].isdigit():
        return ' '.join(sp[1:])
    return addr

def odds(x):
    return 1.0 if x == 1.0 else np.log(x)-np.log(1-x)

def update_odds(x):
    defaut = default_logodds
    val = oddUpdate[x]
    if len(val)!=1:
        defaut[val.keys()] = val
    return pd.Series(defaut)


# In[160]:

print "Calculating logodds"

addresses = sorted(train["Address"].unique())
categories = sorted(train["Category"].unique())
A_counts = train.groupby(["Address"]).size()

C_counts = train.groupby(["Category"]).size()
logoddsPA = dict((A_counts/len(train)).apply(odds))
default_logodds = np.log(C_counts / len(train)) - np.log(1.0 - C_counts / float(len(train)))
oddUpdate = pd.Series(((train.groupby(['Address','Category']).size() / A_counts).apply(odds)))
logodds = {k:update_odds(k) for k in addresses}


# In[161]:

streets = pd.concat([train['Address'].apply(get_street_name), test['Address'].apply(get_street_name)])
street_encoder.fit(streets.unique())


# In[162]:

seasons = dict(zip([1,2,3,4,5,6,7,8,9,10,11,12], ['Winter','Winter','Spring','Spring','Spring','Summer','Summer','Summer','Autumn','Autumn','Autumn','Winter']))
def get_season(d):
    return seasons[d]


# In[171]:

label_encoder = LabelEncoder()
xy_scaler = StandardScaler()

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
    if dt.year < 2010:
        split_year = '2008'
    elif dt.year < 2012:
        split_year = '2010'
    else:
        split_year = '2012'
    return dt.hour, dt.month, split_year, season, isDaytime, is_holiday

def get_features(data, logodds, logoddsPA, train=True):
    df = pd.DataFrame()
    df['hours'], df['months'], df['years'], seasons, df['IsDaytime'], df['IsHoliday'] = zip(*data['Dates'].apply(get_dates))
    df['isWeekend'] = data['DayOfWeek'].apply(lambda x: int(x == 'Saturday' or x == 'Sunday'))
    df = df.join(pd.get_dummies(data['PdDistrict'], prefix='District'))
    df = df.join(pd.get_dummies(data['DayOfWeek'], prefix='Day'))
    # df = df.join(pd.get_dummies(hours, prefix='Hour'))
    # df = df.join(pd.get_dummies(months, prefix='Month'))
    # df = df.join(pd.get_dummies(years, prefix='Years'))
    df = df.join(pd.get_dummies(seasons, prefix='Season'))

    address_features = data['Address'].apply(lambda x: logodds[x])
    address_features.columns = ['logodds' + str(x) for x in range(len(address_features.columns))]
    df = df.join(address_features)
    df['logoddsPA'] = data['Address'].apply(lambda x: logoddsPA[x])

    xy_features = data[['X', 'Y']].join(pd.DataFrame(data['X'] * data['Y']))
    if train:
        xy_scaler.fit(xy_features)
    xy_fitted = xy_scaler.transform(xy_features)
    df['X'] = xy_fitted[:, 0]
    df['Y'] = xy_fitted[:, 1]
    df['XmulY'] = xy_fitted[:, 2]
    df['IsIntersection'] = data['Address'].map(lambda x: int('/' in x))
    df['Street'] = street_encoder.transform(data['Address'].apply(get_street_name))
    labels = None
    if train:
        labels = label_encoder.fit_transform(data['Category'])
    return df, labels


# In[172]:

print "Getting train features"
features, labels = get_features(train, logodds, logoddsPA, True)

num_classes = len(label_encoder.classes_)
num_features = features.shape[1]


# In[165]:

scaler = StandardScaler()
scaler.fit(features)
features_scaled = scaler.transform(features)

del features

# In[174]:

len(labels)


# In[122]:

# X_train, X_test, y_train, y_test = train_test_split(features_scaled, labels, test_size=0.4, random_state=0)


# In[179]:


# In[180]:

def float32(k):
    return np.cast['float32'](k)

class AdjustVariable(object):
    def __init__(self, name, start=0.03, stop=0.001):
        self.name = name
        self.start, self.stop = start, stop
        self.ls = None

    def __call__(self, nn, train_history):
        if self.ls is None:
            self.ls = np.linspace(self.start, self.stop, nn.max_epochs)

        epoch = train_history[-1]['epoch']
        new_value = np.cast['float32'](self.ls[epoch - 1])
        getattr(nn, self.name).set_value(new_value)


# In[185]:

net0 = NeuralNet(layers=[
                    ('input', InputLayer),
                    ('dense0', DenseLayer),
                    ('dropout0', DropoutLayer),
                    ('dense1', DenseLayer),
                    ('output', DenseLayer)],

                 input_shape=(None, num_features),
                 dense0_num_units=100,
                 dropout0_p=0.5,
                 dense1_num_units=100,
                 output_num_units=num_classes,
                 output_nonlinearity=softmax,
                 update=nesterov_momentum,
                 update_learning_rate=0.002,
                 update_momentum=0.9,

                 train_split=TrainSplit(eval_size=0.2),
                 on_epoch_finished=[
                    # AdjustVariable('update_learning_rate', start=0.02, stop=0.001),
                    # AdjustVariable('update_momentum', start=0.9, stop=0.999),
                 ],
                 verbose=1,
                 regression=False,
                 max_epochs=50)


# In[184]:

print 'started', datetime.now()
net0.fit(features_scaled, labels.astype('int32'))
print 'finished', datetime.now()

# In[2]:

import pickle
pickle.dump(net0, open('lasagne_model.pkl', 'wb'))

# net0 = pickle.load('lasagne_model.pkl')

# In[139]:

print 'score', net0.score(features_scaled, labels)


# In[138]:

print 'logloss', log_loss(labels, net0.predict_proba(features_scaled))


# In[126]:

print "Getting test logodds"
new_addresses=sorted(test["Address"].unique())
new_A_counts=test.groupby("Address").size()
only_new=set(new_addresses + addresses) - set(addresses)
only_old=set(new_addresses + addresses) - set(new_addresses)
in_both=set(new_addresses).intersection(addresses)
for addr in only_new:
    PA = new_A_counts[addr] / float(len(test) + len(train))
    logoddsPA[addr] = np.log(PA) - np.log(1. - PA)
    logodds[addr] = copy.deepcopy(default_logodds)
    logodds[addr].index=range(len(categories))
for addr in in_both:
    PA=(A_counts[addr] + new_A_counts[addr]) / float(len(test) + len(train))
    logoddsPA[addr] = np.log(PA) - np.log(1. - PA)


# In[56]:

print "Getting test features"
test_features, _ = get_features(test, logodds, logoddsPA, False)

scaler.fit(test_features)
test_features = scaler.transform(test_features)


# In[18]:

print "Predicting"
proba = net0.predict_proba(test_features)
prediction = pd.DataFrame(proba, columns=label_encoder.classes_)


# In[26]:

print "Writing output"
with gzip.GzipFile('lrv3.csv.gz', mode='w', compresslevel=9) as f:
    prediction.to_csv(f, index_label="Id", na_rep="0", float_format='%11.5f')

