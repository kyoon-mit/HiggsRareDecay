# TensorFlow and tf.keras
import tensorflow as tf

# Helper libraries
import pandas as pd
import uproot

# Decorrelation library
import Disco_tf as disco

print("Tensorflow version: " + tf.__version__)

"""
fashion_mnist = tf.keras.datasets.fashion_mnist
(train_images, train_labels), (test_images, test_labels) = fashion_mnist.load_data()

class_names = ['T-shirt/top', 'Trouser', 'Pullover', 'Dress', 'Coat',
               'Sandal', 'Shirt', 'Sneaker', 'Bag', 'Ankle boot']
"""

####### Part 1. Load ROOT Data #######
variables = ['HCandMass', 'HCandPT', 'goodMeson_iso', 'zepVar']
# variables = signal['events'].keys()

file_format = '/work/submit/kyoon/RareHiggs/data/cat_phi/cat_phi_VBF/test/test_mc{}_VBFcat_{}Cat.root'
sgnmc = 1010
channel = 'Phi'
signal = uproot.open(file_format.format(sgnmc, channel))
background1 = uproot.open(file_format.format(6, channel))
background2 = uproot.open(file_format.format(7, channel))
background3 = uproot.open(file_format.format(8, channel))
background4 = uproot.open(file_format.format(9, channel))

# Convert to pandas dataframe 
signal_data = signal['events'].arrays(variables, library='pd')
background1_data = background1['events'].arrays(variables, library='pd')
background2_data = background2['events'].arrays(variables, library='pd')
background3_data = background3['events'].arrays(variables, library='pd')
background4_data = background4['events'].arrays(variables, library='pd')

# Add truth values
signal_data['y_true'] = 1
background1_data['y_true'] = -1
background2_data['y_true'] = -1
background3_data['y_true'] = -1
background4_data['y_true'] = -1

# Combine seperate dataframes into one object
complete_data = pd.concat([signal_data,
                           background1_data,
                           background2_data,
                           background3_data,
                           background4_data])

# Shuffle in random order
complete_data = complete_data.sample(frac=1).reset_index(drop=True)

####### Part 2. Split data into train and test sets #######
split_level = 0.7
train = complete_data.sample(frac=split_level, random_state=137)
test = complete_data.drop(train.index)

####### Part 3. Build Custom Loss Function #######
def loss_function_with_decorrelation(input_tensor):
    def loss_function(y_true, y_pred):
        return tf.keras.losses.BinaryCrossentropy(y_true, y_pred) + disco.distance_corr(input_tensor['HCandMass'], y_pred)

model = tf.keras.Sequential([
    tf.keras.layers.Flatten(input_shape=(28, 28)),
    tf.keras.layers.Dense(128, activation='relu'),
    tf.keras.layers.Dense(10)
])

model.compile(optimizer='adam',
              loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
              metrics=['accuracy'])

model.fit(train_images, train_labels, epochs=10)
