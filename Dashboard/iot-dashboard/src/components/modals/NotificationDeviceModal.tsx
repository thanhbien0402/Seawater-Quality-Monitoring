import { Button } from '../ui/button'
import {
    Dialog,
    DialogClose,
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogTrigger
} from '../ui/dialog'
import { BadgeAlert } from 'lucide-react'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import NotificationDeviceTable from '@/app/notificationDevice/page'
import SensorRangeTable from '@/app/sensorRanges/page'

export function NotificationDeviceModal({ deviceId }: { deviceId: string }) {
    return (
        <Dialog>
            <DialogTrigger asChild>
                <Button variant="ghost" className="gap-1">
                    <BadgeAlert className="h-4 w-4" />
                    <p>Notification</p>
                </Button>
            </DialogTrigger>
            <DialogContent className="sm:max-w-[80%]">
                <Tabs defaultValue="profile" className="w-full">
                    <TabsList className="grid w-full grid-cols-2">
                        <TabsTrigger value="notification">Notification List</TabsTrigger>
                        <TabsTrigger value="range">Sensor Range</TabsTrigger>
                    </TabsList>
                    <TabsContent value="notification">
                        <DialogHeader>
                            <DialogTitle>Notification List</DialogTitle>
                            <DialogDescription>
                                View your notifications below.
                            </DialogDescription>
                        </DialogHeader>
                        <NotificationDeviceTable deviceId={deviceId} />
                    </TabsContent>
                    <TabsContent value="range">
                        <DialogHeader>
                            <DialogTitle>Sensor Range</DialogTitle>
                            <DialogDescription>
                                All of the sensor range to trigger for notifications
                            </DialogDescription>
                        </DialogHeader>
                        <SensorRangeTable deviceId={deviceId} />
                    </TabsContent>
                </Tabs>
                <DialogFooter>
                    <DialogClose asChild>
                        <Button type="button" variant="default">
                            Close
                        </Button>
                    </DialogClose>
                </DialogFooter>
            </DialogContent>
        </Dialog>
    )
}
