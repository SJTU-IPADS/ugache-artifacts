from torch.utils import data
from common import *
import datetime
import argparse

here = os.path.abspath(os.path.dirname(__file__))
app_dir = os.path.join(here, '../dgl/multi_gpu')

"""
    if log_dir is not None, it will only parse logs
"""


def breakdown_test(log_folder=None):
    if log_folder:
        mock = True
        log_dir = os.path.join(os.path.join(here, f'run-logs/{log_folder}'))
    else:
        mock = False
        log_dir = os.path.join(
            here, f'run-logs/logs_dgl_{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}')

    log_table = LogTable(
        num_row=12,
        num_col=4
    ).update_col_definition(
        col_id=0,
        definition='sample_time'
    ).update_col_definition(
        col_id=1,
        definition='copy_time'
    ).update_col_definition(
        col_id=2,
        definition='train_time'
    ).update_col_definition(
        col_id=3,
        definition='epoch_time'
    ).update_row_definition(
        row_id=0,
        col_range=[0, 3],
        app=App.gcn,
        dataset=Dataset.products
    ).update_row_definition(
        row_id=1,
        col_range=[0, 3],
        app=App.gcn,
        dataset=Dataset.papers100M
    ).update_row_definition(
        row_id=2,
        col_range=[0, 3],
        app=App.gcn,
        dataset=Dataset.twitter
    ).update_row_definition(
        row_id=3,
        col_range=[0, 3],
        app=App.gcn,
        dataset=Dataset.uk_2006_05
    ).update_row_definition(
        row_id=4,
        col_range=[0, 3],
        app=App.graphsage,
        dataset=Dataset.products
    ).update_row_definition(
        row_id=5,
        col_range=[0, 3],
        app=App.graphsage,
        dataset=Dataset.papers100M
    ).update_row_definition(
        row_id=6,
        col_range=[0, 3],
        app=App.graphsage,
        dataset=Dataset.twitter
    ).update_row_definition(
        row_id=7,
        col_range=[0, 3],
        app=App.graphsage,
        dataset=Dataset.uk_2006_05
    ).update_row_definition(
        row_id=8,
        col_range=[0, 3],
        app=App.pinsage,
        dataset=Dataset.products
    ).update_row_definition(
        row_id=9,
        col_range=[0, 3],
        app=App.pinsage,
        dataset=Dataset.papers100M
    ).update_row_definition(
        row_id=10,
        col_range=[0, 3],
        app=App.pinsage,
        dataset=Dataset.twitter
    ).update_row_definition(
        row_id=11,
        col_range=[0, 3],
        app=App.pinsage,
        dataset=Dataset.uk_2006_05
    ).create()

    ConfigList(
    ).select(
        'app',
        [App.gcn, App.graphsage, App.pinsage]
    ).override(
        'num_epoch',
        [3]
    ).override(
        'num_sampling_worker',
        [8]
    ).combo(
        'app',
        [App.pinsage],
        'num_epoch',
        [1]
    ).combo(
        'app',
        [App.gcn, App.graphsage],
        'BOOL_use_gpu_sampling',
        ['use_gpu_sampling']
    ).override(
        'BOOL_pipelining',
        ['no_pipelining']
    ).override(
        'devices',
        ['0'],
        # ).override(
        #     'BOOL_validate_configs',
        #     ['validate_configs']
    ).run(
        appdir=app_dir,
        logdir=log_dir,
        mock=mock
    ).parse_logs(
        logtable=log_table,
        logdir=log_dir
    )


def scalability_test(log_folder=None):
    if log_folder:
        mock = True
        log_dir = os.path.join(os.path.join(here, f'run-logs/{log_folder}'))
    else:
        mock = False
        log_dir = os.path.join(
            here, f'run-logs/logs_dgl_{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}')

    log_table = LogTable(
        num_row=5,
        num_col=1
    ).update_col_definition(
        col_id=0,
        definition='epoch_time'
    ).update_row_definition(
        row_id=0,
        col_range=[0],
        devices='0'
    ).update_row_definition(
        row_id=1,
        col_range=[0],
        devices='0 1'
    ).update_row_definition(
        row_id=2,
        col_range=[0],
        devices='0 1 2 3'
    ).update_row_definition(
        row_id=3,
        col_range=[0],
        devices='0 1 2 3 4 5'
    ).update_row_definition(
        row_id=4,
        col_range=[0],
        devices='0 1 2 3 4 5 6 7'
    ).create()

    ConfigList(
    ).select(
        'app',
        [App.gcn]
    ).select(
        'dataset',
        [Dataset.papers100M]
    ).override(
        'num_epoch',
        [3]
    ).override(
        'devices',
        ['0', '0 1', '0 1 2 3', '0 1 2 3 4 5', '0 1 2 3 4 5 6 7'],
    ).override(
        'BOOL_use_gpu_sampling',
        ['use_gpu_sampling']
        # ).override(
        #     'BOOL_validate_configs',
        #     ['validate_configs']
    ).run(
        appdir=app_dir,
        logdir=log_dir,
        mock=mock
    ).parse_logs(
        logtable=log_table,
        logdir=log_dir
    )


if __name__ == '__main__':
    argparser = argparse.ArgumentParser("DGL runner")
    argparser.add_argument('-l', '--log-folder', default=None)
    args = argparser.parse_args()

    # breakdown_test(args.log_folder)
    scalability_test(args.log_folder)
